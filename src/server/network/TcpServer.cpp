//
// Created by fran on 27/07/2026.
//

#include "TcpServer.hpp"
#include <iostream>

TcpServer::TcpServer(boost::asio::io_context &ioContext, const int port, TopicCache &topicCache)
    : m_port(port)
    , m_nextSessionId(1)
    , m_topicCache(topicCache)
    , m_ioContext(ioContext)
    , m_acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<boost::asio::ip::port_type>(port)))
{
}

void TcpServer::run()
{
    std::cout << "Tcp server running on port " << m_port << std::endl;
    startAccept();
}

void TcpServer::startAccept()
{
    std::shared_ptr<TcpConnection> newConnection = TcpConnection::create(m_ioContext);

    newConnection->initCallbacks(std::bind(&TcpServer::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1),
        std::bind(&TcpServer::onConnectionError, this, std::placeholders::_1, std::placeholders::_2));
    m_acceptor.async_accept(newConnection->getSocket(),
        std::bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void TcpServer::handleAccept(const std::shared_ptr<TcpConnection> &newConnection, const boost::system::error_code &error)
{
    if (error)
        std::cerr << "Error in new connection accept handling" << error.message() << std::endl;
    else {
        addConnection(newConnection);
        newConnection->start();
    }
    startAccept();
}

void TcpServer::sendMessageToAllClients(const std::string &messageToSend) const
{
    for (const auto &connection : m_activeConnections)
        connection->sendMessage(messageToSend);
}

void TcpServer::addConnection(const std::shared_ptr<TcpConnection> &newConnection)
{
    m_activeConnections.insert(newConnection);
    std::cout << "New client connected, we have " << m_activeConnections.size() << " active client(s) so far" << std::endl;
}

void TcpServer::removeConnection(const std::shared_ptr<TcpConnection> &connectionToRemove)
{
    unsubscribeConnectionFromAllTopics(connectionToRemove);

    const auto it = m_activeConnections.find(connectionToRemove);
    if (it != m_activeConnections.end())
        m_activeConnections.erase(it);
    std::cout << "After a client disconnection we have " << m_activeConnections.size() << " client(s) remaining" << std::endl;
}

void TcpServer::onMessageReceived(const std::shared_ptr<TcpConnection> &connection, const std::string &message)
{
    std::cout << "TcpServer received the following message: " << message << std::endl;
    const std::vector<std::string> messageParts = splitString(message, ' ');

    if (messageParts.empty()) {
        // print invalid message
        return;
    }
    if (messageParts[0] == "HELLO") {
        onNewClientConnected(connection, messageParts);
        return;
    }
    if (connection->state() != SessionState::Ready) {
        connection->sendMessage("425 NOT_READY");
        return;
    }
    if (messageParts[0] == "TOPICS") {
        onTopicsRequested(connection);
        return;
    }
    if (messageParts[0] == "SUB") {
        onSubscriptionRequested(connection, messageParts);
        return;
    }
    if (messageParts[0] == "UNSUB") {
        onUnsubscriptionRequested(connection, messageParts);
        return;
    }
    if (messageParts[0] == "ALERT") {
        onAlertRequested(connection, messageParts);
        return;
    }
    if (messageParts[0] == "BYE") {
        onByeRequested(connection);
        return;
    }
    connection->sendMessage("400 BAD_REQUEST");
}

void TcpServer::onConnectionError(const std::shared_ptr<TcpConnection> &connection,
    const boost::system::error_code &error)
{
    if (error) {
        std::cerr << "TcpServer connection error: " << error.message() << std::endl;
        removeConnection(connection);
    }
}

void TcpServer::onNewClientConnected(const std::shared_ptr<TcpConnection> &connection,
    const std::vector<std::string> &helloMessageParts)
{
    if (helloMessageParts.size() != 2) {
        connection->sendMessage("400 BAD_REQUEST - Invalid hello message");
        return;
    }
    const std::vector<std::string> udpMessageParts = splitString(helloMessageParts[1], '=');
    if (udpMessageParts.size() != 2 || udpMessageParts[0] != "udp_port"){
        connection->sendMessage("400 BAD_REQUEST - No udp_port provided");
        return;
    }
    const std::optional<std::uint16_t> udpPort = parsePort(udpMessageParts[1]);
    if (!udpPort) {
        connection->sendMessage("400 BAD_REQUEST - Invalid udp_port");
        return;
    }
    const auto clientAddress = connection->getSocket().remote_endpoint().address();
    const boost::asio::ip::udp::endpoint udpEndpoint(clientAddress, *udpPort);

    connection->setSessionId(m_nextSessionId++);
    connection->setUdpPort(*udpPort);
    connection->setUdpEndpoint(udpEndpoint);
    connection->setState(SessionState::Ready);
    connection->sendMessage("200 OK session=" + std::to_string(connection->sessionId()));
}

void TcpServer::onTopicsRequested(const std::shared_ptr<TcpConnection> &connection) const
{
    const std::vector<TopicDescriptor> topics = m_topicCache.topics();
    std::string reply = "210 TOPICS";
    for (const auto &[name, type, schema] : topics)
        reply += " " + name + ":" + streamTypeToString(type);
    connection->sendMessage(reply);
}

void TcpServer::onSubscriptionRequested(const std::shared_ptr<TcpConnection> &connection,
    std::vector<std::string> subMessageParts)
{
    if (subMessageParts.size() != 2) {
        connection->sendMessage("400 BAD_REQUEST - Invalid subscription message");
        return;
    }

    const std::string &topicName = subMessageParts[1];

    if (!m_topicCache.hasTopic(topicName)) {
        connection->sendMessage("404 UNKNOWN_TOPIC " + topicName);
        return;
    }
    subscribeConnectionToTopic(connection, topicName);

    unsigned int id = m_topicCache.topicId(topicName);
    std::vector<std::string> schema = m_topicCache.topicSchema(topicName);
    connection->sendMessage("201 SUBSCRIBED " + formatSubscribedTopicInfo(topicName, id, schema));
}

void TcpServer::onUnsubscriptionRequested(const std::shared_ptr<TcpConnection> &connection,
    std::vector<std::string> unsubMessageParts)
{
}

void TcpServer::onAlertRequested(const std::shared_ptr<TcpConnection> &connection,
    std::vector<std::string> alertMessageParts)
{
}

void TcpServer::onByeRequested(const std::shared_ptr<TcpConnection> &connection)
{
}

std::string TcpServer::streamTypeToString(const StreamType type)
{
    switch (type) {
        case StreamType::FINANCE:
            return "FINANCE";
        case StreamType::WEATHER:
            return "WEATHER";
        case StreamType::SENSOR:
            return "SENSOR";
    }
    return "UNKNOWN";
}

void TcpServer::subscribeConnectionToTopic(const std::shared_ptr<TcpConnection> &connection, const std::string &topicName)
{
    connection->subscribeToTopic(topicName);
    if (m_topicSubscriptions.find(topicName) == m_topicSubscriptions.end())
        m_topicSubscriptions.emplace(topicName, std::unordered_set<std::shared_ptr<TcpConnection>>());
    m_topicSubscriptions[topicName].insert(connection);
}

void TcpServer::unsubscribeConnectionFromTopic(const std::shared_ptr<TcpConnection> &connection, 
    const std::string &topicName)
{
    connection->unsubscribeFromTopic(topicName);
    if (m_topicSubscriptions.find(topicName) == m_topicSubscriptions.end())
        return;
    std::unordered_set<std::shared_ptr<TcpConnection>> &subscribers = m_topicSubscriptions[topicName];
    if (subscribers.find(connection) != subscribers.end())
        subscribers.erase(connection);
}

void TcpServer::unsubscribeConnectionFromAllTopics(const std::shared_ptr<TcpConnection> &connection)
{
    std::unordered_set<std::string> subscribedTopics = connection->subscribedTopics();

    for (const auto &topic : subscribedTopics)
        unsubscribeConnectionFromTopic(connection, topic);
}

std::string TcpServer::formatSubscribedTopicInfo(const std::string &topicName, unsigned int id, const std::vector<std::string> &schema)
{
    std::string message = "topic_name=" + topicName + " topic_id=" + std::to_string(id) + " type=[";

    for (const auto &field: schema)
        message += field + ",";
    message.pop_back();
    message += "]";
    return message;
}
