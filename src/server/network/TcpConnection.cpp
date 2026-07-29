//
// Created by fran on 28/07/2026.
//

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
    m_messageToSend = "New client accepted by the server\n";
    boost::asio::async_write(m_socket, boost::asio::buffer(m_messageToSend),
        std::bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    m_socket.async_read_some(boost::asio::buffer(m_messageToRead),
        std::bind(&TcpConnection::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::initCallbacks(std::function<void(std::shared_ptr<TcpConnection>, std::string)> onMessageReceived,
    std::function<void(std::shared_ptr<TcpConnection>)> onDisconnect,
    std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> onError)
{
    m_onMessageReceived = std::move(onMessageReceived);
    m_onDisconnect = std::move(onDisconnect);
    m_onError = std::move(onError);
}

void TcpConnection::sendMessage(const std::string &messageToSend)
{
    m_messageToSend = messageToSend;
    boost::asio::async_write(m_socket, boost::asio::buffer(m_messageToSend),
        std::bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

TcpConnection::TcpConnection(boost::asio::io_context &ioContext)
    : m_messageToRead()
    , m_socket(ioContext)
    , m_onDisconnect(nullptr)
    , m_onMessageReceived(nullptr)
    , m_onError(nullptr)
{
}

void TcpConnection::handleWrite(const boost::system::error_code &error, size_t bytes_transferred)
{
    m_messageToSend.clear();
}

void TcpConnection::handleRead(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (error == boost::asio::error::eof && m_onDisconnect)
        m_onDisconnect(shared_from_this());
    else if (error && m_onError)
        m_onError(shared_from_this(), error);
}
