//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TCPSERVER_HPP
#define FLOWBROKER_TCPSERVER_HPP

#include "TcpConnection.hpp"

class TcpServer {
public:
    TcpServer(boost::asio::io_context& ioContext, int port);
    void run();
    void startAccept();
    void handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error);
private:
    int m_port;
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
};


#endif //FLOWBROKER_TCPSERVER_HPP
