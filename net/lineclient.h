#pragma once

#include <sigc++/sigc++.h>
#include <gtkmm.h>

#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "utils.h"

#define SMPORT 24203
#define BUFLEN 4096
#define MAX_SEND_LENGTH BUFLEN-2

using boost::asio::ip::tcp;

enum ConnectionResult { CR_MALFORMED_HOST, CR_OFFLINE, CR_SUCCESS };


class LineClient
{
    public:
        
        // these signals are emitted in main thread
        sigc::signal<void, ConnectionResult> connection_attempt_signal;
        sigc::signal<void, bool> disconnection_signal;    // if true, disconnect was intended by user
        sigc::signal<void, string> line_received_signal;

        LineClient();

        // called from main thread
        void begin_connect(string hostname);
        void begin_disconnect();
        void send(string line);

    private:
        // used in worker thread
        boost::asio::io_service io_service;
        tcp::socket* socket;
        tcp::resolver resolver;
        boost::thread net_thread;
        
        string connect_hostname;
        char receive_array[BUFLEN];
        char send_array[BUFLEN];
        string partial_line;
        bool is_sending;

        void thread_code();
        void begin_receive();
        void on_receive(const boost::system::error_code& error, std::size_t num_bytes);
        void do_send(string line);
        void on_send(const boost::system::error_code& error, std::size_t num_bytes);
        void do_disconnect(bool intentional);
        void do_intentional_disconnect();

        // used in both threads
        Glib::Dispatcher line_receive_dispatch, connect_dispatch, disconnect_dispatch;
        boost::lockfree::spsc_queue<string> lines_to_send;
        boost::lockfree::spsc_queue<string> received_lines;
        ConnectionResult last_connection_result;
        bool intentional_disconnect;

        // used in ui thread to fire signals
        void on_line_receive_dispatch();
        void on_connect_disp();
        void on_disconnect_disp();
};

