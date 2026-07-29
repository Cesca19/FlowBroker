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
    void initCallbacks(std::function<void(std::shared_ptr<TcpConnection>, std::string)> onMessageReceived,
        std::function<void(std::shared_ptr<TcpConnection>)> onDisconnect, std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> onError);
    void sendMessage(const std::string &messageToSend);
private:
    TcpConnection(boost::asio::io_context &ioContext);
    void handleWrite(const boost::system::error_code &error, size_t bytes_transferred);
    void handleRead(const boost::system::error_code &error, size_t bytes_transferred);

    std::string m_messageToSend;
    std::array<char, 512> m_messageToRead;
    boost::asio::ip::tcp::socket m_socket;
    std::function<void(std::shared_ptr<TcpConnection>)> m_onDisconnect;
    std::function<void(std::shared_ptr<TcpConnection>, std::string)> m_onMessageReceived;
    std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> m_onError;
};


#endif //FLOWBROKER_TCPCONNECTION_HPP
