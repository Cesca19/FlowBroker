//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TCPSERVER_HPP
#define FLOWBROKER_TCPSERVER_HPP

#include <unordered_set>
#include "TcpConnection.hpp"

class TcpServer {
public:
    TcpServer(boost::asio::io_context& ioContext, int port);
    void run();
    void startAccept();
    void handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error);
    void sendMessageToAllClients(const std::string &messageToSend) const;
private:
    void addConnection(const std::shared_ptr<TcpConnection> &newConnection);
    void removeConnection(const std::shared_ptr<TcpConnection> &connectionToRemove);
    void onMessageReceived(const std::shared_ptr<TcpConnection> &connection, const std::string &message);
    void onConnectionError(const std::shared_ptr<TcpConnection> &connection, const boost::system::error_code &error);

    int m_port;
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<std::shared_ptr<TcpConnection>> m_activeConnections;
};


#endif //FLOWBROKER_TCPSERVER_HPP
