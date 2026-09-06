//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TCPSERVER_HPP
#define FLOWBROKER_TCPSERVER_HPP

#include <unordered_set>
#include "TcpConnection.hpp"
#include "NetworkUtils.hpp"
#include "../topics/TopicCache.hpp"

class TcpServer {
public:
    TcpServer(boost::asio::io_context& ioContext, int port, TopicCache &topicCache);
    void run();
    void startAccept();
    void handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error);
    void sendMessageToAllClients(const std::string &messageToSend) const;
private:
    void addConnection(const std::shared_ptr<TcpConnection> &newConnection);
    void removeConnection(const std::shared_ptr<TcpConnection> &connectionToRemove);
    void onMessageReceived(const std::shared_ptr<TcpConnection> &connection, const std::string &message);
    void onConnectionError(const std::shared_ptr<TcpConnection> &connection, const boost::system::error_code &error);

    void onNewClientConnected(const std::shared_ptr<TcpConnection> &connection, const std::vector<std::string> &helloMessageParts);
    void onTopicsRequested(const std::shared_ptr<TcpConnection> &connection) const;
    void onSubscriptionRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> subMessageParts);
    void onUnsubscriptionRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> unsubMessageParts);
    void onAlertRequested(const std::shared_ptr<TcpConnection> &connection, std::vector<std::string> alertMessageParts);
    void onByeRequested(const std::shared_ptr<TcpConnection> &connection);

    static std::string streamTypeToString(StreamType type);

    int m_port;
    int m_nextSessionId;
    TopicCache &m_topicCache;
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::unordered_set<std::shared_ptr<TcpConnection>> m_activeConnections;
};


#endif //FLOWBROKER_TCPSERVER_HPP
