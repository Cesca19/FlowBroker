//
// Created by fran on 01/09/2026.
//

#include "ClientWindow.hpp"

ClientWindow::ClientWindow(const std::string &host, const int tcpPort, const int udpPort, QWidget *parent)
    : QWidget(parent)
    , m_tcpPort(tcpPort)
    , m_udpPort(udpPort)
    , m_host(host)
    , m_tcpClient(nullptr)
    , m_connectBtn(nullptr)
    , m_graphsLayout(nullptr)
    , m_tcpConnectionState(ConnectionState::Disconnected)
{
    auto* rootLayout = new QVBoxLayout(this);
    auto* graphsContainer = new QWidget();
    m_graphsLayout = new QVBoxLayout(graphsContainer);
    m_connectBtn = new QPushButton("Connect");
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(graphsContainer);
    scrollArea->setWidgetResizable(true);   // container follows the scroll area width

    auto *formLayout = new QFormLayout();
    m_hostEdit = new QLineEdit(QString::fromStdString(host));
    m_tcpPortEdit = new QLineEdit(QString::number(tcpPort));
    m_udpPortEdit = new QLineEdit(QString::number(udpPort));
    formLayout->addRow("Host", m_hostEdit);
    formLayout->addRow("TCP port", m_tcpPortEdit);
    formLayout->addRow("UDP port", m_udpPortEdit);
    rootLayout->addLayout(formLayout);

    rootLayout->addWidget(m_connectBtn);
    rootLayout->addWidget(scrollArea);
    resize(800, 800);

    m_tcpClient = new TcpClient(this);
    connect(m_tcpClient, &TcpClient::addMessage, this, &ClientWindow::onAddMessageRequested);
    connect(m_tcpClient, &TcpClient::newTopicReceived, this, &ClientWindow::onNewTopicReceived);
    connect(m_tcpClient, &TcpClient::connectionStateChanged, this, &ClientWindow::onTcpConnectionStateChanged);
    connect(m_connectBtn, &QPushButton::clicked, this, &ClientWindow::onConnectButtonClicked);
}

void ClientWindow::onAddMessageRequested(const QString &messageTitle, const QString &messageContent, const MessageType messageType)
{
    switch (messageType) {
        case MessageType::Info:
            QMessageBox::information(this, messageTitle, messageContent);
            break;
        case MessageType::Warning:
            QMessageBox::warning(this, messageTitle, messageContent);
            break;
        case MessageType::Error:
            QMessageBox::critical(this, messageTitle, messageContent);
            break;
    }
}

void ClientWindow::onConnectButtonClicked()
{
    if (m_tcpConnectionState == ConnectionState::Connected) {
        m_tcpClient->disconnectFromServer();
        return;
    }

    const std::optional<std::string> host = parseHost(m_hostEdit->text().toStdString());
    if (!host) {
        onAddMessageRequested(tr("FlowBroker Client"), tr("Invalid host address."), MessageType::Error);
        return;
    }
    const std::optional<std::uint16_t> tcpPort = parsePort(m_tcpPortEdit->text().toStdString());
    if (!tcpPort) {
        onAddMessageRequested(tr("FlowBroker Client"), tr("Invalid TCP port."), MessageType::Error);
        return;
    }
    const std::optional<std::uint16_t> udpPort = parsePort(m_udpPortEdit->text().toStdString());
    if (!udpPort) {
        onAddMessageRequested(tr("FlowBroker Client"), tr("Invalid UDP port."), MessageType::Error);
        return;
    }

    m_host = *host;
    m_tcpPort = *tcpPort;
    m_udpPort = *udpPort;
    m_tcpClient->connectToServer(m_host, m_tcpPort);
}

void ClientWindow::onNewTopicReceived(const QString &topicName, const qint64 tsMs, const double value)
{
    const TopicGraph* graph = findOrCreateGraph(topicName);
    graph->addPoint(tsMs, value);
}

void ClientWindow::onTcpConnectionStateChanged(const ConnectionState connectionState)
{
    const bool isEditable = connectionState == ConnectionState::Disconnected;
    m_hostEdit->setEnabled(isEditable);
    m_tcpPortEdit->setEnabled(isEditable);
    m_udpPortEdit->setEnabled(isEditable);

    switch (connectionState) {
        case ConnectionState::Connecting:
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to connect...");
            break;
        case ConnectionState::Connected:
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Disconnect");
            break;
        case ConnectionState::Closing:
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to disconnect...");
            break;
        case ConnectionState::Disconnected:
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Connect");
            clearGraphs();
            break;
        default:
            break;
    }
    m_tcpConnectionState = connectionState;
}

TopicGraph * ClientWindow::findOrCreateGraph(const QString &topicName)
{
    const auto it = m_graphsByTopic.find(topicName);
    if (it != m_graphsByTopic.end())
        return it.value();

    auto* graph = new TopicGraph(topicName);
    m_graphsLayout->addWidget(graph);
    m_graphsByTopic.insert(topicName, graph);
    return graph;
}

void ClientWindow::clearGraphs()
{
    for (TopicGraph* graph : m_graphsByTopic)
        graph->deleteLater();
    m_graphsByTopic.clear();
}
