//
// Created by fran on 28/07/2026.
//

#ifndef FLOWBROKER_TCPCONNECTION_HPP
#define FLOWBROKER_TCPCONNECTION_HPP

#include <queue>
#include <memory>
#include <boost/asio.hpp>

enum class SessionState { Connected, Ready };

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    static std::shared_ptr<TcpConnection> create(boost::asio::io_context &ioContext);
    boost::asio::ip::tcp::socket& getSocket();
    void start();
    void initCallbacks(std::function<void(std::shared_ptr<TcpConnection>, std::string)> onMessageReceived,
        std::function<void(std::shared_ptr<TcpConnection>)> onDisconnect, std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> onError);
    void sendMessage(const std::string &messageToSend);

    void setSessionId(int sessionId);
    void setUdpPort(std::uint16_t udpPort);
    void setState(SessionState sessionState);
    void setUdpEndpoint(const boost::asio::ip::udp::endpoint &udpEndpoint);
    int sessionId() const;
    std::uint16_t udpPort() const;
    SessionState state() const;
    boost::asio::ip::udp::endpoint udpEndpoint() const;
private:
    void readMessage();
    void sendNextMessage();
    TcpConnection(boost::asio::io_context &ioContext);
    void handleWrite(const boost::system::error_code &error, size_t bytes_transferred);
    void handleRead(const boost::system::error_code &error, size_t bytes_transferred);

    int m_sessionId;
    std::uint16_t m_udpPort;
    SessionState m_sessionState;
    std::string m_pendingMessage;
    boost::asio::ip::udp::endpoint m_udpEndpoint;
    std::queue<std::string> m_messagesToSend;
    std::array<char, 512> m_messageToRead;
    boost::asio::ip::tcp::socket m_socket;
    std::function<void(std::shared_ptr<TcpConnection>)> m_onDisconnect;
    std::function<void(std::shared_ptr<TcpConnection>, std::string)> m_onMessageReceived;
    std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> m_onError;
};


#endif //FLOWBROKER_TCPCONNECTION_HPP
