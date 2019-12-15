#include "lineclient.h"
#include "log.h"


LineClient::LineClient() : resolver(io_service), lines_to_send(50), received_lines(50)
{
    log("LineClient::LineClient()");
    socket = nullptr;
    last_connection_result = CR_SUCCESS;
    is_sending = false;

    line_receive_dispatch.connect(sigc::mem_fun(this, &LineClient::on_line_receive_dispatch));
    connect_dispatch.connect(sigc::mem_fun(this, &LineClient::on_connect_disp));
    disconnect_dispatch.connect(sigc::mem_fun(this, &LineClient::on_disconnect_disp));
}


void LineClient::begin_connect(string hostname)
{
    log("LineClient::begin_connect(" + hostname);
    connect_hostname = hostname;
    partial_line = "";
    intentional_disconnect = false;

    string useless;
    while(received_lines.pop(useless));
    while(lines_to_send.pop(useless));

    for(int i=0; i<BUFLEN; i++)
    {
        receive_array[i] = 0;
        send_array[i] = 0;
    }

    net_thread.join();
    net_thread = boost::thread(&LineClient::thread_code, this);
}


void LineClient::on_connect_disp()
{
    connection_attempt_signal.emit(last_connection_result);
}


void LineClient::begin_disconnect()
{
    log("LineClient::begin_disconnect()");
    io_service.post(boost::bind(&LineClient::do_intentional_disconnect, this));
}


void LineClient::do_intentional_disconnect()
{
    do_disconnect(true);
}


void LineClient::do_disconnect(bool intentional)
{
    intentional_disconnect = intentional;
    socket->close();
    io_service.stop();
}


void LineClient::on_disconnect_disp()
{
    disconnection_signal.emit(intentional_disconnect);
}


void LineClient::on_line_receive_dispatch()
{
    string next;
    while(received_lines.pop(next))
        line_received_signal.emit(next);
}


void LineClient::begin_receive()
{
    auto receive_buffer = boost::asio::buffer(receive_array, BUFLEN);
    auto bound = boost::bind(boost::mem_fn(&LineClient::on_receive), this, _1, _2); 
    socket->async_receive(receive_buffer, bound);
}


void LineClient::on_receive(const boost::system::error_code& error, std::size_t num_bytes)
{
    if(error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
    {
        do_disconnect(false);
        return;
    }

    bool first = true;
    int last_i = -2;
    
    for(int i=0; i<num_bytes; i++)
    {
        if(receive_array[i] == '\r')
        {
            // get chunk of text between newlines
            int newpos = last_i + 2;      // + 1 if \r only not \r\n
            if(newpos >= num_bytes)
                break;

            char *pointer = receive_array + newpos; 
            int length = i - last_i;
            string chunk(pointer, length); //TODO check pointer arithmetic 

            string line;
            if(first)
            {
                line = partial_line + chunk;
                first = false;
            }

            else
                line = chunk;

            received_lines.push(line);

            last_i = i; 
        }
    }

    line_receive_dispatch.emit();

    int newpos = last_i + 2;
    char *pointer = receive_array + newpos;  // +1 if \r only
    int length = num_bytes - last_i;
    
    if(newpos >= num_bytes || length < 1)
        partial_line = "";
    else
        partial_line = string(pointer, length);

    begin_receive();
}


void LineClient::send(string line)
{
    line += "\r\n";

    while(line.length() > MAX_SEND_LENGTH)
    {
        lines_to_send.push(line.substr(0, MAX_SEND_LENGTH));
        line = line.substr(MAX_SEND_LENGTH);
    }

    lines_to_send.push(line);
    lines_to_send.pop(line);
    io_service.post(boost::bind(&LineClient::do_send, this, line));
}


void LineClient::do_send(string line)
{
    if(is_sending)
        return;

    is_sending = true;

    int len;
    len = line.length();

    for(int i=0; i<len; i++)
        send_array[i] = line[i];
    
    auto bound = boost::bind(boost::mem_fn(&LineClient::on_send), this, _1, _2);
    socket->async_send(boost::asio::buffer(send_array, len), bound);
}


void LineClient::on_send(const boost::system::error_code& error, std::size_t num_bytes)
{
    is_sending = false;
    if(error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
    {
        do_disconnect(false);
        return;
    }

    string next_line;
    if(lines_to_send.pop(next_line))
        do_send(next_line);
}


void LineClient::thread_code()
{
    log("LineClient::thread_code()");
    io_service.reset();   // called restart() in latest version, TODO get that version
    socket = new tcp::socket(io_service);
    tcp::resolver::query query(connect_hostname, std::to_string(SMPORT)); 
    tcp::resolver::iterator it;

    try { it = resolver.resolve(query); }
    catch(boost::system::system_error err)
    {
        log("LineClient unable to resolve host");
        last_connection_result = CR_MALFORMED_HOST;
        connect_dispatch.emit();
        return;
    }
    
    tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;
    
    while(error && it != end)
    {
        socket->close();
        socket->connect(*it++, error);
    }

    if(error)
    {
        log("LineClient could not connect");
        last_connection_result = CR_OFFLINE;
        connect_dispatch.emit();
        return;
    }

    log("LineClient connected, running io loop");
    last_connection_result = CR_SUCCESS;
    connect_dispatch.emit();

    begin_receive();
    io_service.run();  // blocks here until disconnected

    log("LineClient finished");
    delete socket;
    disconnect_dispatch.emit();
}



