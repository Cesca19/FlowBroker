//
// Created by fran on 02/09/2026.
//

#include <iostream>
#include "ClientSession.hpp"

ClientSession::ClientSession(QObject *parent)
    : QObject(parent)
    , m_tcpConnection(nullptr)
    , m_udpReceiver(nullptr)
{
    m_tcpConnection = new TcpClientConnection(this);
    connect(m_tcpConnection, &TcpClientConnection::addMessage, this, &ClientSession::addMessage);
    connect(m_tcpConnection, &TcpClientConnection::connectionStateChanged, this, &ClientSession::tcpConnectionStateChanged);
    connect(m_tcpConnection, &TcpClientConnection::messageReceived, this, &ClientSession::handleTcpServerMessage);

    m_udpReceiver = new UdpReceiver(this);
    connect(m_udpReceiver, &UdpReceiver::updConnectionEstablished, this, &ClientSession::udpConnectionEstablished);
    connect(m_udpReceiver, &UdpReceiver::udpConnectionFailed, this, &ClientSession::udpConnectionFailed);
    connect(m_udpReceiver, &UdpReceiver::messageReceived, this, &ClientSession::handleUdpReceiverMessage);
}

void ClientSession::connectTcpClient(const std::string &host, const std::uint16_t port) const
{
    m_tcpConnection->connectToServer(host, port);
}

void ClientSession::disconnectTcpClient() const
{
    m_tcpConnection->disconnectFromServer();
}

void ClientSession::connectUdpReceiver(int udpPort) const
{
    m_udpReceiver->start(udpPort);
}

void ClientSession::disconnectUdpReceiver() const
{
   m_udpReceiver->close();
}

void ClientSession::handleTcpServerMessage(const QString &message)
{
    const QStringList parts = message.split(' ');

    if (parts.isEmpty())
        return;
    if (parts.size() >= 2 && parts[0] == "200" && parts[1] == "OK") {
        onOkMessageReceived(parts);
        return;
    }
    if (parts[0] == "201") {
        // 201 SUBSCRIBED topic_name=<name> topic_id=<int> type=<TYPE> fields=[<name>,<name>,...]
        return;
    }
    if (parts[0] == "202") {
        // 202 UNSUBSCRIBED ...
        return;
    }
    if (parts[0] == "203") {
        // 203 ALERT_SET id=<int>
        return;
    }
    if (parts.size() >= 2 && parts[0] == "210" && parts[1] == "TOPICS") {
        onTopicsListReceived(parts);
        return;
    }
    if (parts[0] == "300") {
        // 300 ALERT id=<int> <topic> <field> <value> <op> <threshold>
        return;
    }
    if (parts[0] == "400" || parts[0] == "404"  || parts[0] == "425") {
        // client error messsages
        return;
    }
    std::cout << "ClientSession - Unknown message: " << message.toStdString() << std::endl;

}

void ClientSession::handleUdpReceiverMessage(const QString &messageContent)
{
    const QStringList parts = messageContent.split(';');

    if (parts.isEmpty())
        return;
    const QString type = parts[0];
    if (type == "TOPIC")
        onNewTopicSnapshotReceived(parts);
    // std::cout << "-" << message.toStdString() << "-" << std::endl;
}

void ClientSession::sendHello(const int udpPort) const
{
    m_tcpConnection->sendMessage("HELLO udp_port=" + std::to_string(udpPort));
}

void ClientSession::getTopics() const
{
    m_tcpConnection->sendMessage("TOPICS");
}

void ClientSession::subscribeToTopic(const std::string &topicName) const
{
    m_tcpConnection->sendMessage("SUB " + topicName);
}

void ClientSession::unSubscribeFromTopic(const std::string &topicName) const
{
    m_tcpConnection->sendMessage("UNSUB " + topicName);
}

void ClientSession::createAlert(const std::string &topicName, const std::string &topicField,
    const std::string &op, const double value) const
{
    const std::string alertMessage = "ALERT " + topicName + " " + topicField + " " + op + " "
        + QString::number(value, 'f', 6).toStdString();
    m_tcpConnection->sendMessage(alertMessage);
}

void ClientSession::sendBye() const
{
    m_tcpConnection->sendMessage("BYE");
}

void ClientSession::onOkMessageReceived(QStringList messageParts)
{
    if (messageParts.size() == 3)
        onSessionIdReceived(messageParts);
    if (messageParts.size() == 2) {
        // bye request validated
    }
}

void ClientSession::onSessionIdReceived(QStringList messageParts)
{
    QString sessionStr = messageParts[2];
    QStringList sessionSplit = sessionStr.split("=");

    if (sessionSplit.size() != 2 || sessionSplit[0] != "session")
        return;
    emit tcpSessionReady(sessionSplit[1].toInt());
}

void ClientSession::onTopicsListReceived(QStringList messageParts)
{
    std::vector<TopicDescriptor> availableTopics;

    for (int i = 2; i < messageParts.size(); i++) {
        QStringList topicDesc = messageParts[i].split(":");

        if (topicDesc.size() != 2)
            continue;
        availableTopics.push_back(TopicDescriptor{ topicDesc[0].toStdString(), streamTypeFromString(topicDesc[1]) });
    }
    emit topicsListReady(availableTopics);
}

void ClientSession::onNewTopicSnapshotReceived(const QStringList &message)
{
    // Expect: TOPIC:name:ts:value:average:min:max  -> 7 fields
    if (message.size() != 7)
        return;   // skip

    const QString& topicName = message[1];
    const qint64 tsNs = message[2].toLongLong();
    const double value = message[3].toDouble();
    // const double min = message[5].toDouble();
    // const double max = message[6].toDouble();

    emit newTopicReceived(topicName, tsNs / 1'000'000, value);
}

StreamType ClientSession::streamTypeFromString(QString name)
{
    if (name == "FINANCE")
        return StreamType::FINANCE;
    if (name == "WEATHER")
        return StreamType::WEATHER;
    if (name == "SENSOR")
        return StreamType::SENSOR;
    return StreamType::UNKNOWN;
}
