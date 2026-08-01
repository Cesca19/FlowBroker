//
// Created by fran on 27/07/2026.
//

#include "TcpServer.hpp"
#include <iostream>

TcpServer::TcpServer(boost::asio::io_context &ioContext, const int port)
    : m_port(port)
    , m_ioContext(ioContext)
    , m_acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}

void TcpServer::run()
{
    startAccept();
}

void TcpServer::startAccept()
{
    std::shared_ptr<TcpConnection> newConnection = TcpConnection::create(m_ioContext);

    newConnection->initCallbacks(std::bind(&TcpServer::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1),
        std::bind(&TcpServer::onConnectionError, this, std::placeholders::_1, std::placeholders::_2));
    m_acceptor.async_accept(newConnection->getSocket(),
        std::bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void TcpServer::handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error)
{
    if (error)
        std::cerr << "Error in new connection accept handling" << error.message() << std::endl;
    else {
        addConnection(newConnection);
        newConnection->start();
    }
    startAccept();
}

void TcpServer::sendMessageToAllClients(const std::string &messageToSend) const
{
    for (const auto &connection : m_activeConnections)
        connection->sendMessage(messageToSend);
}

void TcpServer::addConnection(const std::shared_ptr<TcpConnection> &newConnection)
{
    m_activeConnections.insert(newConnection);
    std::cout << "New client connected, we have " << m_activeConnections.size() << " active client(s) so far" << std::endl;
}

void TcpServer::removeConnection(const std::shared_ptr<TcpConnection> &connectionToRemove)
{
    const auto it = m_activeConnections.find(connectionToRemove);
    if (it != m_activeConnections.end())
        m_activeConnections.erase(it);
    std::cout << "After a client disconnection we have " << m_activeConnections.size() << " client(s) remaining" << std::endl;
}

void TcpServer::onMessageReceived(const std::shared_ptr<TcpConnection> &connection, const std::string &message)
{
    std::cout << "TcpServer received the following message: " << message << std::endl;
}

void TcpServer::onConnectionError(const std::shared_ptr<TcpConnection> &connection,
    const boost::system::error_code &error)
{
    if (error) {
        std::cerr << "TcpServer connection error: " << error.message() << std::endl;
        removeConnection(connection);
    }
}
