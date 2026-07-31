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
    , m_isConnected(false)
    , m_connectBtn(nullptr)
    , m_tcpSocket(nullptr)
{
    setFixedSize(320, 240);
    m_connectBtn = std::make_unique<QPushButton>("Connect", this);
    m_connectBtn->setCheckable(true);
    m_connectBtn->setGeometry(100, 120, 120, 30);

    connect(m_connectBtn.get(), SIGNAL(clicked(bool)), this, SLOT(onConnectButtonClicked(bool)));
    connect(this, SIGNAL(quit()), QApplication::instance(), SLOT(quit()));
    initConnection();
}

void QTcpClient::initConnection()
{
    m_tcpSocket = std::make_unique<QTcpSocket>(this);

    connect(m_tcpSocket.get(), SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(m_tcpSocket.get(), SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket.get(), SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this , SLOT(onConnectionError(QAbstractSocket::SocketError)));
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

void QTcpClient::onConnectButtonClicked(bool checked) const
{
    m_connectBtn->setEnabled(false);
    if (!m_isConnected) {
        m_connectBtn->setText("Trying to connect...");
        connectToServer();
    } else {
        m_connectBtn->setText("Trying to disconnect...");
        disconnectFromServer();
    }
}

void QTcpClient::onConnected() {
    std::cout << "Connected successfully" << std::endl;
    m_connectBtn->setEnabled(true);
    m_connectBtn->setChecked(true);
    m_connectBtn->setText("Disconnect");
    m_isConnected = true;
}

void QTcpClient::onDisconnected() {
    std::cout << "Disconnected successfully" << std::endl;
    m_connectBtn->setEnabled(true);
    m_connectBtn->setChecked(false);
    m_connectBtn->setText("Connect");
    m_isConnected = false;
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
    m_isConnected = false;
    m_connectBtn->setEnabled(true);
    m_connectBtn->setChecked(false);
    m_connectBtn->setText("Connect");
}