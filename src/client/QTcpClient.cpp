//
// Created by fran on 29/07/2026.
//

#include "QTcpClient.hpp"

#include <iostream>
#include <ostream>

QTcpClient::QTcpClient(const std::string &host, const int port, QWidget *parent)
    : QWidget(parent)
    , m_port(port)
    , m_host(host)
    , m_connectBtn(nullptr)
    , m_tcpSocket(nullptr)
    , m_socketState(QAbstractSocket::UnconnectedState)
{
    setFixedSize(320, 240);
    m_connectBtn = std::make_unique<QPushButton>("Connect", this);
    m_connectBtn->setGeometry(100, 120, 120, 30);

    connect(m_connectBtn.get(), SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(this, SIGNAL(quit()), QApplication::instance(), SLOT(quit()));
    initConnection();
}

void QTcpClient::initConnection()
{
    m_tcpSocket = std::make_unique<QTcpSocket>(this);
    m_socketState = m_tcpSocket->state();

    connect(m_tcpSocket.get(), SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(m_tcpSocket.get(), SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket.get(), SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this , SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
}

void QTcpClient::connectToServer() const
{
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(m_host.c_str(), m_port);
}

void QTcpClient::disconnectFromServer() const
{
    m_tcpSocket->disconnectFromHost();
}

void QTcpClient::onConnectButtonClicked() const
{
    if (m_socketState == QAbstractSocket::UnconnectedState)
        connectToServer();
    else if (m_socketState == QAbstractSocket::ConnectedState)
        disconnectFromServer();
}

void QTcpClient::onConnected() const
{
    std::cout << "Connected successfully" << std::endl;
}

void QTcpClient::onDisconnected() const
{
    std::cout << "Disconnected successfully" << std::endl;
}

void QTcpClient::onMessageReceived()
{
    m_buffer.append(m_tcpSocket->readAll());

    qsizetype newlineIndex;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        std::string line = m_buffer.first(newlineIndex).trimmed().toStdString();

        m_buffer.remove(0, newlineIndex + 1);
        if (line.empty())
            continue;
        std::cout << "-" << line << "-" << std::endl;
    }
}

void QTcpClient::onConnectionError(const QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            QMessageBox::information(this, tr("FlowBroker Client"),
                                     tr("The remote host closed the connection. "));
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("FlowBroker Client"),
                                     tr("The host was not found. Please check the "
                                        "host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("FlowBroker Client"),
                                     tr("The connection was refused by the peer. "
                                        "Make sure the Flowbroker server is running, "
                                        "and check that the host name and port "
                                        "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("FlowBroker Client"),
                                     tr("The following error occurred: %1.")
                                     .arg(m_tcpSocket->errorString()));
    }
}

void QTcpClient::onSocketStateChanged(const QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
        case QAbstractSocket::UnconnectedState:
            std::cout << "Entering unconnected state" << std::endl;
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Connect");
            break;
        case QAbstractSocket::HostLookupState: //	The socket is performing a host name lookup.
            std::cout << "Trying to lookup host" << std::endl;
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to connect...");
            break;
        case QAbstractSocket::ConnectingState: //	The socket has started establishing a connection.
            std::cout << "Trying to connect..." << std::endl;
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to connect...");
            break;
        case QAbstractSocket::ConnectedState:
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Disconnect");
            break;
        case QAbstractSocket::BoundState: //	The socket is bound to an address and port.
            std::cout << "BoundState" << std::endl;
            break;
        case QAbstractSocket::ClosingState: //	The socket is about to close (data may still be waiting to be written).
            std::cout << "Trying to close connection" << std::endl;
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to disconnect...");
            break;
        default:
            break;
    }
    m_socketState = socketState;
}
