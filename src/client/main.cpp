//
// Created by fran on 06/07/2026.
//

#include <QtGui>
#include <QApplication>
#include <QLabel>

#include <iostream>
#include <boost/asio.hpp>

void tcp_client(const std::string& host, std::string port) 
{
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        for (;;) {
            std::array<char, 128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            std::cout.write(buf.data(), len);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

int main(int ac, char **av) 
{
    if (ac < 3) {
        std::cerr << "Usage: " << av[0] << " <host> <port>" << std::endl;
        return 1;
    }
    tcp_client(av[1], av[2]);
    // QApplication app(argc, argv);
    // QLabel label("Hello from the client !");
    // label.show();
    // return app.exec();
}
