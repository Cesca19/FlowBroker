//
// Created by fran on 28/07/2026.
//

#include <ctime>
#include "TcpConnection.hpp"

std::shared_ptr<TcpConnection> TcpConnection::create(boost::asio::io_context &ioContext)
{
    return std::shared_ptr<TcpConnection>(new TcpConnection(ioContext));
}

boost::asio::ip::tcp::socket & TcpConnection::getSocket()
{
    return m_socket;
}

void TcpConnection::start()
{
    m_message = makeDaytimeString();
    boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
        std::bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

TcpConnection::TcpConnection(boost::asio::io_context &ioContext)
    : m_socket(ioContext)
{
}

void TcpConnection::handleWrite(const boost::system::error_code &error, size_t bytes_transferred)
{
}

std::string TcpConnection::makeDaytimeString()
{
    const std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}
