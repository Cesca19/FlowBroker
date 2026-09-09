//
// Created by fran on 31/08/2026.
//

#ifndef FLOWBROKER_UDPSENDER_HPP
#define FLOWBROKER_UDPSENDER_HPP

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>
#include <iostream>


/**
 * @class UdpSender
 * @brief The server's single send-only UDP socket.
 *
 * UDP is connectionless, so there is one socket for all clients: the same socket
 * sends to a different endpoint each time. This class never receives; it only
 * pushes bytes to a given target.
 *
 * Thread-safety: sendTo() may be called from any thread. It posts the actual
 * send onto a strand, so all socket access is serialized on the io_context and
 * two sends never overlap, without any mutex.
 */
class UdpSender {
public:
    UdpSender(boost::asio::io_context &ioContext, int port);
    
    /**
     * @brief Send a payload to one client endpoint.
     * @param target      Where to send (built from the client's IP + announced UDP port).
     * @param dataToSend  The bytes to send; kept alive by shared_ptr until the send completes.
     */
    void sendTo(const boost::asio::ip::udp::endpoint &target, std::shared_ptr<std::string> dataToSend);

private:
    void handleSend(std::shared_ptr<std::string> data, const boost::system::error_code &error, std::size_t bytesTransferred);

    boost::asio::io_context &m_ioContext;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};


#endif //FLOWBROKER_UDPSENDER_HPP
