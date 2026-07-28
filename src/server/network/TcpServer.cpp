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
    m_acceptor.async_accept(newConnection->getSocket(),
        std::bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void TcpServer::handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error)
{
    if (error)
        std::cerr << "Error in new connection accept handling" << error.message() << std::endl;
    else
        newConnection->start();
    startAccept();
}
