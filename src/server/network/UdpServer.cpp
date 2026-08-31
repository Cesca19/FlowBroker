//
// Created by fran on 31/08/2026.
//

#include "UdpServer.hpp"


std::string make_daytime_string()
{
    std::time_t now = std::time(0);
    return std::ctime(&now);
}

UdpServer::UdpServer(boost::asio::io_context &ioContext, const int port)
    : m_port(port)
    , m_ioContext(ioContext)
    , m_socket(m_ioContext, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), m_port))
{
    startReceive();
}

void UdpServer::startReceive()
{
    m_socket.async_receive_from(boost::asio::buffer(m_receiveBuffer), m_remoteEndpoint,
    std::bind(&UdpServer::handleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void UdpServer::handleReceive(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        std::shared_ptr<std::string> message(new std::string(make_daytime_string()));
        m_socket.async_send_to(boost::asio::buffer(*message), m_remoteEndpoint,
            std::bind(&UdpServer::handleSend, this, message, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        startReceive();
    }
}

void UdpServer::handleSend(std::shared_ptr<std::string> message, const boost::system::error_code &error, std::size_t bytes_transferred)
{
}