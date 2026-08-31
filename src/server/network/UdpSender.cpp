//
// Created by fran on 31/08/2026.
//

#include "UdpSender.hpp"


UdpSender::UdpSender(boost::asio::io_context &ioContext, const int port)
    : m_ioContext(ioContext)
    , m_socket(m_ioContext, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
    , m_strand(boost::asio::make_strand(m_ioContext))
{
}

void UdpSender::sendTo(const boost::asio::ip::udp::endpoint &target, std::shared_ptr<std::string> dataToSend)
{
    boost::asio::post(m_strand, [this, target, dataToSend]() {
        m_socket.async_send_to(boost::asio::buffer(*dataToSend), target,
            std::bind(&UdpSender::handleSend, this, dataToSend, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    });
}

void UdpSender::handleSend(std::shared_ptr<std::string> data, const boost::system::error_code &error, std::size_t bytesTransferred)
{
    if (error)
        std::cerr << "UDP send failed: " << error.message() << std::endl;
}