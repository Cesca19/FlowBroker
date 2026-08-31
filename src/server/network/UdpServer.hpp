//
// Created by fran on 31/08/2026.
//

#ifndef FLOWBROKER_UDPSERVER_HPP
#define FLOWBROKER_UDPSERVER_HPP

#include <boost/asio.hpp>
#include <functional>
#include <ctime>
#include <memory>
#include <string>

class UdpServer {
public:
    UdpServer(boost::asio::io_context &ioContext, int port);
private:
    void startReceive();
    void handleReceive(const boost::system::error_code &error, std::size_t bytes_transferred);
    void handleSend(std::shared_ptr<std::string> message, const boost::system::error_code &error, std::size_t bytes_transferred);

    int m_port;
    boost::asio::io_context &m_ioContext;
    std::array<char, 1> m_receiveBuffer;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_remoteEndpoint;
};


#endif //FLOWBROKER_UDPSERVER_HPP
