//
// Created by fran on 29/07/2026.
//

#include "TcpClient.hpp"
#include <iostream>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , m_tcpSocket(nullptr)
    , m_socketState(QAbstractSocket::UnconnectedState)
{
    initConnection();
}

void TcpClient::initConnection()
{
    m_tcpSocket = new QTcpSocket(this);
    m_socketState = m_tcpSocket->state();

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this , SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
}

void TcpClient::connectToServer(const std::string &host, const std::uint16_t port) const
{
    if (m_socketState != QAbstractSocket::UnconnectedState)
        return;
    std::cout << "Connecting to: " << host << ":" << port << std::endl;
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host.c_str(), port);
}

void TcpClient::disconnectFromServer() const
{
    if (m_socketState == QAbstractSocket::ConnectedState)
        m_tcpSocket->disconnectFromHost();
}

void TcpClient::onConnected() const
{
    std::cout << "Connected successfully" << std::endl;
}

void TcpClient::onDisconnected() const
{
    std::cout << "Disconnected successfully" << std::endl;
}

void TcpClient::onMessageReceived()
{
    m_buffer.append(m_tcpSocket->readAll());

    qsizetype newlineIndex;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        QString line = m_buffer.first(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);
        if (line.isEmpty())
            continue;
        handleServerMessage(line);
    }
}

void TcpClient::handleServerMessage(const QString &message)
{
    const QStringList parts = message.split(':');

    if (parts.isEmpty())
        return;
    const QString type = parts[0];
    if (type == "TOPIC")
        onNewTopicSnapshotReceived(parts);
    // std::cout << "-" << message.toStdString() << "-" << std::endl;
}

void TcpClient::onNewTopicSnapshotReceived(const QStringList &message)
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

void TcpClient::onConnectionError(const QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            emit addMessage(tr("FlowBroker Client"),
                tr("The remote host closed the connection. "));
            break;
        case QAbstractSocket::HostNotFoundError:
            emit addMessage(tr("FlowBroker Client"),
                tr("The host was not found. Please check the "
                                    "host name and port settings."), MessageType::Error);
            break;
        case QAbstractSocket::ConnectionRefusedError:
            emit addMessage(tr("FlowBroker Client"),
                tr("The connection was refused by the peer. "
                                "Make sure the Flowbroker server is running, "
                                "and check that the host name and port "
                                "settings are correct."), MessageType::Warning);
            break;
        default:
            emit addMessage(tr("FlowBroker Client"),
                tr("The following error occurred: %1.").arg(m_tcpSocket->errorString()),
                MessageType::Error);
    }
}

void TcpClient::onSocketStateChanged(const QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
        case QAbstractSocket::UnconnectedState:
            std::cout << "Entering unconnected state" << std::endl;
            emit connectionStateChanged(ConnectionState::Disconnected);
            break;
        case QAbstractSocket::HostLookupState:
            //	The socket is performing a host name lookup.
            std::cout << "Trying to lookup host" << std::endl;
            emit connectionStateChanged(ConnectionState::Connecting);
            break;
        case QAbstractSocket::ConnectingState:
            //	The socket has started establishing a connection.
            std::cout << "Trying to connect..." << std::endl;
            emit connectionStateChanged(ConnectionState::Connecting);
            break;
        case QAbstractSocket::ConnectedState:
            emit connectionStateChanged(ConnectionState::Connected);
            break;
        case QAbstractSocket::BoundState:
            //	The socket is bound to an address and port.
            std::cout << "BoundState" << std::endl;
            break;
        case QAbstractSocket::ClosingState:
            //	The socket is about to close (data may still be waiting to be written).
            std::cout << "Trying to close connection" << std::endl;
            emit connectionStateChanged(ConnectionState::Closing);
            break;
        default:
            break;
    }
    m_socketState = socketState;
}
