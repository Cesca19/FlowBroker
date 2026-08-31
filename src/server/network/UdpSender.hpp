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

class UdpSender {
public:
    UdpSender(boost::asio::io_context &ioContext, int port);
    void sendTo(const boost::asio::ip::udp::endpoint &target, std::shared_ptr<std::string> dataToSend);

private:
    void handleSend(std::shared_ptr<std::string> data, const boost::system::error_code &error, std::size_t bytesTransferred);

    boost::asio::io_context &m_ioContext;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};


#endif //FLOWBROKER_UDPSENDER_HPP
