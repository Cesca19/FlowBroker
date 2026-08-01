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
    , m_graphsLayout(nullptr)
{
    auto* rootLayout = new QVBoxLayout(this);
    auto* graphsContainer = new QWidget();
    m_graphsLayout = new QVBoxLayout(graphsContainer);
    m_connectBtn = new QPushButton("Connect");
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(graphsContainer);
    scrollArea->setWidgetResizable(true);   // container follows the scroll area width

    rootLayout->addWidget(m_connectBtn);
    rootLayout->addWidget(scrollArea);
    resize(800, 800);
    connect(m_connectBtn, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(this, SIGNAL(quit()), QApplication::instance(), SLOT(quit()));
    initConnection();
}

void QTcpClient::initConnection()
{
    m_tcpSocket = new QTcpSocket(this);
    m_socketState = m_tcpSocket->state();

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this , SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
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
        QString line = m_buffer.first(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);
        if (line.isEmpty())
            continue;
        handleServerMessage(line);
    }
}

void QTcpClient::handleServerMessage(const QString &message)
{
    const QStringList parts = message.split(':');

    if (parts.isEmpty())
        return;
    const QString type = parts[0];
    if (type == "TOPIC")
        onNewTopicSnapshotReceived(parts);
    // std::cout << "-" << message.toStdString() << "-" << std::endl;
}

void QTcpClient::onNewTopicSnapshotReceived(const QStringList &message)
{
    // Expect: TOPIC:name:ts:value:average:min:max  -> 7 fields
    if (message.size() != 7)
        return;   // skip

    const QString topicName = message[1];
    const qint64 tsNs = message[2].toLongLong();
    const double value = message[3].toDouble();
    const double min = message[5].toDouble();
    const double max = message[6].toDouble();

    const TopicGraph* graph = findOrCreateGraph(topicName);
    graph->addPoint(tsNs / 1'000'000, value, min, max);
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
            clearGraphs();
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

TopicGraph * QTcpClient::findOrCreateGraph(const QString &topicName)
{
    auto it = m_graphsByTopic.find(topicName);
    if (it != m_graphsByTopic.end())
        return it.value();

    auto* graph = new TopicGraph(topicName);
    m_graphsLayout->addWidget(graph);
    m_graphsByTopic.insert(topicName, graph);
    return graph;
}

void QTcpClient::clearGraphs()
{
    for (TopicGraph* graph : m_graphsByTopic)
        graph->deleteLater();
    m_graphsByTopic.clear();
}
