//
// Created by fran on 02/09/2026.
//

#include <iostream>
#include "ClientSession.hpp"

ClientSession::ClientSession(QObject *parent)
    : QObject(parent)
    , m_tcpConnection(nullptr)
{
    m_tcpConnection = new TcpClientConnection(this);
    connect(m_tcpConnection, &TcpClientConnection::addMessage, this, &ClientSession::addMessage);
    connect(m_tcpConnection, &TcpClientConnection::connectionStateChanged, this, &ClientSession::tcpConnectionStateChanged);

    connect(m_tcpConnection, &TcpClientConnection::messageReceived, this, &ClientSession::handleTcpServerMessage);
}

void ClientSession::connectTcpClient(const std::string &host, const std::uint16_t port) const
{
    m_tcpConnection->connectToServer(host, port);
}

void ClientSession::disconnectTcpClient() const
{
    m_tcpConnection->disconnectFromServer();
}

void ClientSession::handleTcpServerMessage(const QString &message)
{
    const QStringList parts = message.split(' ');

    if (parts.isEmpty())
        return;
    if (parts[0] == "200") {
        // OK message, after hello or bye
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
    if (parts[0] == "210") {
        // 210 TOPICS <name>:<TYPE> <name>:<TYPE> ...
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
    // const QStringList parts = message.split(';');

    // if (parts.isEmpty())
    //     return;
    // const QString type = parts[0];
    // if (type == "TOPIC")
    //     onNewTopicSnapshotReceived(parts);
    // // std::cout << "-" << message.toStdString() << "-" << std::endl;
}

void ClientSession::sendHello(const int m_udpPort) const
{
    m_tcpConnection->sendMessage("HELLO udp_port=" + std::to_string(m_udpPort));
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
    const std::string alertMessage = "ALERT " + topicName + " " +
        topicField + " " +  op  + " " + std::to_string(value);
    m_tcpConnection->sendMessage(alertMessage);
}

void ClientSession::sendBye() const
{
    m_tcpConnection->sendMessage("BYE");
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
