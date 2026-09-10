//
// Created by fran on 28/07/2026.
//

#ifndef FLOWBROKER_TCPCONNECTION_HPP
#define FLOWBROKER_TCPCONNECTION_HPP

#include <queue>
#include <memory>
#include <unordered_set>
#include <boost/asio.hpp>

/// Session state: CONNECTED = TCP up but no valid HELLO yet, READY = HELLO done.
enum class SessionState { Connected, Ready };

/**
 * @class TcpConnection
 * @brief It represents a single TCP connection to a client.
 *
 * Owns the client's socket, turns the incoming  messages byte stream into complete
 * CRLF-terminated lines, and sends replies. It also holds the per-client
 * session data: session id, session state, and the UDP endpoint where this
 * client's data stream will be pushed. The server identifies a client by its
 * TcpConnection object.
 *
 * Lifetime: managed by shared_ptr and enable_shared_from_this. Every async
 * operation binds shared_from_this() into its handler, so the object stays
 * alive as long as a read or write is pending, and is destroyed only once no
 * operation and no external owner remain.
 *
 * Created through the static create(); the constructor is private so a
 * TcpConnection can only ever exist as a shared_ptr.
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    /// Create a connection as a shared_ptr (the only way to build one).
    static std::shared_ptr<TcpConnection> create(boost::asio::io_context &ioContext);
    
    /// The underlying socket of this connection.
    boost::asio::ip::tcp::socket& getSocket();
    
    /// Begin reading from the client (launch the first async read).
    void start();
    
    /**
     * @brief Wire up the callbacks the server reacts to.
     * @param onMessageReceived  Called with each complete line received.
     * @param onDisconnect       Called when the client closes the connection.
     * @param onError            Called on a socket error.
     */
    void initCallbacks(std::function<void(std::shared_ptr<TcpConnection>, std::string)> onMessageReceived,
        std::function<void(std::shared_ptr<TcpConnection>)> onDisconnect, std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> onError);
    
    /// Dispatch a line to send to the client (CRLF is appended); sends immediately if idle.
    void sendMessage(const std::string &messageToSend);

    void setSessionId(int sessionId);
    void setUdpPort(std::uint16_t udpPort);
    void setState(SessionState sessionState);
    void setUdpEndpoint(const boost::asio::ip::udp::endpoint &udpEndpoint);
    int sessionId() const;
    std::uint16_t udpPort() const;
    SessionState state() const;
    boost::asio::ip::udp::endpoint udpEndpoint() const;
    void subscribeToTopic(const std::string &topicName);
    void unsubscribeFromTopic(const std::string &topicName);
    std::unordered_set<std::string> subscribedTopics() const;
private:
    void readMessage();
    void sendNextMessage();
    TcpConnection(boost::asio::io_context &ioContext);
    void handleWrite(const boost::system::error_code &error, size_t);
    void handleRead(const boost::system::error_code &error, size_t bytes_transferred);

    int m_sessionId;
    std::uint16_t m_udpPort;
    SessionState m_sessionState;
    std::string m_pendingMessage;
    boost::asio::ip::udp::endpoint m_udpEndpoint;
    std::unordered_set<std::string> m_subscribedTopics;
    std::queue<std::string> m_messagesToSend;
    std::array<char, 512> m_messageToRead;
    boost::asio::ip::tcp::socket m_socket;
    std::function<void(std::shared_ptr<TcpConnection>)> m_onDisconnect;
    std::function<void(std::shared_ptr<TcpConnection>, std::string)> m_onMessageReceived;
    std::function<void(std::shared_ptr<TcpConnection>, boost::system::error_code)> m_onError;
};


#endif //FLOWBROKER_TCPCONNECTION_HPP
