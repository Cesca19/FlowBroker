//
// Created by fran on 28/07/2026.
//

#ifndef FLOWBROKER_TCPCONNECTION_HPP
#define FLOWBROKER_TCPCONNECTION_HPP

#include <memory>
#include <boost/asio.hpp>

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    static std::shared_ptr<TcpConnection> create(boost::asio::io_context &ioContext);
    boost::asio::ip::tcp::socket& getSocket();
    void start();
private:
    TcpConnection(boost::asio::io_context &ioContext);
    void handleWrite(const boost::system::error_code &error, size_t bytes_transferred);
    static std::string makeDaytimeString();

    std::string m_message;
    boost::asio::ip::tcp::socket m_socket;
};


#endif //FLOWBROKER_TCPCONNECTION_HPP
