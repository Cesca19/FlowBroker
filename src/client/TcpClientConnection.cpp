//
// Created by fran on 29/07/2026.
//

#include "TcpClientConnection.hpp"
#include <iostream>

TcpClientConnection::TcpClientConnection(QObject *parent)
    : QObject(parent)
    , m_tcpSocket(nullptr)
    , m_socketState(QAbstractSocket::UnconnectedState)
{
    initConnection();
}

void TcpClientConnection::initConnection()
{
    m_tcpSocket = new QTcpSocket(this);
    m_socketState = m_tcpSocket->state();

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this , SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
}

void TcpClientConnection::connectToServer(const std::string &host, const std::uint16_t port) const
{
    if (m_socketState != QAbstractSocket::UnconnectedState)
        return;
    std::cout << "Connecting to: " << host << ":" << port << std::endl;
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host.c_str(), port);
}

void TcpClientConnection::disconnectFromServer() const
{
    if (m_socketState == QAbstractSocket::ConnectedState)
        m_tcpSocket->disconnectFromHost();
}

void TcpClientConnection::onConnected()
{
    std::cout << "Connected successfully" << std::endl;
    sendMessage("Hello");
}

void TcpClientConnection::onDisconnected() const
{
    std::cout << "Disconnected successfully" << std::endl;
}

void TcpClientConnection::onMessageReceived()
{
    m_buffer.append(m_tcpSocket->readAll());

    qsizetype newlineIndex;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        QString line = m_buffer.first(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);
        if (line.isEmpty())
            continue;
        emit messageReceived(line);
    }
}

void TcpClientConnection::onConnectionError(const QAbstractSocket::SocketError socketError)
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

void TcpClientConnection::onSocketStateChanged(const QAbstractSocket::SocketState socketState)
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

void TcpClientConnection::sendMessage(const std::string &message)
{
    if (m_socketState != QAbstractSocket::ConnectedState) {
        emit addMessage("FlowBroker Client", "Failed to send message to the server", MessageType::Warning);
        return;
    }
    const std::string line = message + "\r\n";
    m_tcpSocket->write(line.data(), static_cast<qint64>(line.size()));
}
