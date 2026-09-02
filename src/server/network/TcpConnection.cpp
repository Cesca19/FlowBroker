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
    // sendMessage("New client accepted by the server");
    readMessage();
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
    const std::string message = messageToSend + "\r\n";
    if (m_messagesToSend.empty()) {
        m_messagesToSend.push(message);
        sendNextMessage();
    }
    else
        m_messagesToSend.push(message);
}

void TcpConnection::readMessage()
{
    m_socket.async_read_some(boost::asio::buffer(m_messageToRead),
        std::bind(&TcpConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::sendNextMessage()
{
    if (!m_messagesToSend.empty())
        boost::asio::async_write(m_socket, boost::asio::buffer(m_messagesToSend.front()),
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
    if (error && m_onError) {
        m_onError(shared_from_this(), error);
        return;
    }
    if (error)
        return;
    m_messagesToSend.pop();
    if (!m_messagesToSend.empty())
        sendNextMessage();
}

void TcpConnection::handleRead(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (error == boost::asio::error::eof && m_onDisconnect) {
        m_onDisconnect(shared_from_this());
        return;
    }
    if (error && m_onError) {
        m_onError(shared_from_this(), error);
        return;
    }
    m_pendingMessage += std::string(m_messageToRead.data(), bytes_transferred);

    while (m_pendingMessage.find('\n') != std::string::npos) {
        std::string line = m_pendingMessage.substr(0, m_pendingMessage.find('\n'));
        m_pendingMessage.erase(0, m_pendingMessage.find('\n') + 1);
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty() && m_onMessageReceived)
            m_onMessageReceived(shared_from_this(), line);
    }
    readMessage();
}