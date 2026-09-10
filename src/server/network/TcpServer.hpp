//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TCPSERVER_HPP
#define FLOWBROKER_TCPSERVER_HPP

#include <unordered_set>
#include "TcpConnection.hpp"
#include "NetworkUtils.hpp"
#include "../topics/TopicCache.hpp"

/**
 * @class TcpServer
 * @brief Accepts tcp clients and dispatches their commands.
 *
 * Runs an asynchronous accept loop (so it serves several clients at once), keeps
 * one TcpConnection per client, and interprets the text protocol: HELLO, TOPICS,
 * SUB, UNSUB, ALERT, BYE.
 * It enforces the session rule: only HELLO is allowed before a client is READY;
 * anything else earlier gets 425 NOT_READY.
 *
 * Runs on the server thread (the one driving the io_context).
 */
class TcpServer {
public:
    TcpServer(boost::asio::io_context& ioContext, int port, TopicCache &topicCache);
    
    /// Log the listening port and launch the first accept.
    void run();

    /// Launch one async accept for the next client.
    void startAccept();
    void handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error);
    void sendMessageToAllClients(const std::string &messageToSend) const;
    std::vector<boost::asio::ip::udp::endpoint> getUdpEndpointsForTopic(const std::string &topicName) const;
private:
    void addConnection(const std::shared_ptr<TcpConnection> &newConnection);
    void removeConnection(const std::shared_ptr<TcpConnection> &connectionToRemove);
    
    /// Parse one received line and dispatch to the matching command handler.
    void onMessageReceived(const std::shared_ptr<TcpConnection> &connection, const std::string &message);
    void onConnectionError(const std::shared_ptr<TcpConnection> &connection, const boost::system::error_code &error);

    void onNewClientConnected(const std::shared_ptr<TcpConnection> &connection, const std::vector<std::string> &helloMessageParts);
    void onTopicsRequested(const std::shared_ptr<TcpConnection> &connection) const;
    void onSubscriptionRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> subMessageParts);
    void onUnsubscriptionRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> unsubMessageParts);
    void onAlertRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> alertMessageParts);
    void onByeRequested(const std::shared_ptr<TcpConnection> &connection);

    static std::string streamTypeToString(StreamType type);
    void subscribeConnectionToTopic(const std::shared_ptr<TcpConnection> &connection, const std::string &topicName);
    void unsubscribeConnectionFromTopic(const std::shared_ptr<TcpConnection> &connection, const std::string &topicName);
    void unsubscribeConnectionFromAllTopics(const std::shared_ptr<TcpConnection> &connection);
    static std::string formatSubscribedTopicInfo(const std::string &topicName, unsigned int id, 
        StreamType type, const std::vector<std::string> &schema);

    int m_port;
    int m_nextSessionId;
    TopicCache &m_topicCache;
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<std::shared_ptr<TcpConnection>> m_activeConnections;
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<TcpConnection>>> m_topicSubscriptions;
};


#endif //FLOWBROKER_TCPSERVER_HPP
