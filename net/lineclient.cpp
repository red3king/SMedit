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
    log("LineClient firing signal!");
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

    log("LineClient got data (not seg yet) : " + string(receive_array, BUFLEN) ); 

    int i;
    for(i=0; i<num_bytes; i++)
    {
        if(receive_array[i] == '\r')
            break;
    }

    bool found = i < num_bytes;
    
    // todo - break this line splitting out into its own testable function
    // not efficient whatsoever
    if(found)
    {
        string rest_of_line(receive_array, i);    

        string remainder;
        if(i >= num_bytes-2)
            remainder = "";

        else
        {
            int rem_len = num_bytes - i - 2;
            string remainder(&receive_array[i+2], rem_len);
        }

        string line = partial_line + rest_of_line;
        partial_line = remainder;

        received_lines.push(line);
        log("LineClient got line " + line + ", sending signal");
        line_receive_dispatch.emit();
    }

    else
    {
        if(partial_line.length() > 3*BUFLEN)
            partial_line = "";

        string more(receive_array, num_bytes);
        partial_line = partial_line + more;
    }

    begin_receive();
}


void LineClient::send(string line)
{
    line += "\r\n";
    log("LineClient::send(" + line + ")");

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



