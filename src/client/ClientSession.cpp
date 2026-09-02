//
// Created by fran on 02/09/2026.
//

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
    const QStringList parts = message.split(':');

    if (parts.isEmpty())
        return;
    const QString type = parts[0];
    if (type == "TOPIC")
        onNewTopicSnapshotReceived(parts);
    // std::cout << "-" << message.toStdString() << "-" << std::endl;
}

void ClientSession::onNewTopicSnapshotReceived(const QStringList &message)
{
    // Expect: TOPIC:name:ts:value:average:min:max  -> 7 fields
    if (message.size() != 7)
        return;   // skip

    const QString& topicName = message[1];
    const qint64 tsNs = message[2].toLongLong();
    const double value = message[3].toDouble();
    const double min = message[5].toDouble();
    const double max = message[6].toDouble();

    emit newTopicReceived(topicName, tsNs / 1'000'000, value);
}
