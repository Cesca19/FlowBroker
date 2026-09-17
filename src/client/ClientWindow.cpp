//
// Created by fran on 01/09/2026.
//

#include "ClientWindow.hpp"
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDebug>

ClientWindow::ClientWindow(const std::string &host, const int tcpPort, const int udpPort, QWidget *parent)
    : QWidget(parent)
    , m_tcpPort(tcpPort)
    , m_udpPort(udpPort)
    , m_host(host)
    , m_tcpConnectionState(ConnectionState::Disconnected)
    , m_connectBtn(nullptr)
    , m_graphsLayout(nullptr)
    , m_topicsBox(nullptr)
    , m_topicsLayout(nullptr)
{
    auto* rootLayout = new QVBoxLayout(this);
    auto* graphsContainer = new QWidget();
    m_graphsLayout = new QVBoxLayout(graphsContainer);
    m_connectBtn = new QPushButton("Connect");
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(graphsContainer);
    scrollArea->setWidgetResizable(true);   // container follows the scroll area width
    
    m_topicsBox = new QGroupBox(tr("Available Topics"));
    m_topicsLayout = new QGridLayout(m_topicsBox);

    auto *formLayout = new QFormLayout();
    m_hostEdit = new QLineEdit(QString::fromStdString(host));
    m_tcpPortEdit = new QLineEdit(QString::number(tcpPort));
    m_udpPortEdit = new QLineEdit(QString::number(udpPort));
    
    formLayout->addRow("Host", m_hostEdit);
    formLayout->addRow("TCP port", m_tcpPortEdit);
    formLayout->addRow("UDP port", m_udpPortEdit);
    formLayout->addRow(m_connectBtn);

    auto *rowLayout = new QHBoxLayout();
    rowLayout->addLayout(formLayout);
    rowLayout->addWidget(m_topicsBox);
    
    rootLayout->addLayout(rowLayout);
    rootLayout->addWidget(scrollArea);
    resize(800, 800);

    m_clientSession = new ClientSession(this);
    connect(m_clientSession, &ClientSession::addMessage, this, &ClientWindow::onAddMessageRequested);
    connect(m_clientSession, &ClientSession::newTopicReceived, this, &ClientWindow::onNewTopicReceived);
    connect(m_clientSession, &ClientSession::tcpConnectionStateChanged, this, &ClientWindow::onTcpConnectionStateChanged);
    connect(m_clientSession, &ClientSession::udpConnectionEstablished, this, &ClientWindow::onUdpConnectionEstablished);
    connect(m_clientSession, &ClientSession::udpConnectionFailed, this, &ClientWindow::onUdpConnectionFailed);
    connect(m_clientSession, &ClientSession::tcpSessionReady, this, &ClientWindow::onTcpSessionReady);
    connect(m_clientSession, &ClientSession::topicsListReady, this, &ClientWindow::onTopicsListReady);

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
        m_clientSession->disconnectTcpClient();
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
    m_clientSession->connectUdpReceiver(m_udpPort);
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

    m_tcpConnectionState = connectionState;
    switch (connectionState) {
        case ConnectionState::Connecting:
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to connect...");
            break;
        case ConnectionState::Connected:
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Disconnect");
            onTcpClientConnected();
            break;
        case ConnectionState::Closing:
            m_connectBtn->setEnabled(false);
            m_connectBtn->setText("Trying to disconnect...");
            break;
        case ConnectionState::Disconnected:
            m_connectBtn->setEnabled(true);
            m_connectBtn->setText("Connect");
            m_clientSession->disconnectUdpReceiver();
            clearTopicButtons();
            clearGraphs();
            break;
        default:
            break;
    }
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

void ClientWindow::removeGraph(const QString &topicName)
{
    TopicGraph *graph = m_graphsByTopic.take(topicName); // take remove the item from the hash
    if (graph)
        graph->deleteLater();
}

void ClientWindow::clearGraphs()
{
    for (TopicGraph* graph : m_graphsByTopic)
        graph->deleteLater();
    m_graphsByTopic.clear();
}

void ClientWindow::onUdpConnectionEstablished()
{
    m_clientSession->connectTcpClient(m_host, m_tcpPort);
}

void ClientWindow::onUdpConnectionFailed(const QString &errorMessage)
{
    onAddMessageRequested("FlowBroker Client", errorMessage + "\n Try using another udp port", MessageType::Error);
}

void ClientWindow::onTcpClientConnected() const
{
    m_clientSession->sendHello(m_udpPort);
}

void ClientWindow::onTcpSessionReady(int sessionId)
{
    qDebug() << "Session Id" << sessionId;
    m_clientSession->getTopics();
}

void ClientWindow::onTopicsListReady(std::vector<TopicDescriptor> availableTopics)
{
    clearTopicButtons();

    static constexpr int s_topicColumns = 3;
    int index = 0;
    for (const TopicDescriptor &topic : availableTopics) {
        const QString topicName = QString::fromStdString(topic.name);
        auto *button = new QCheckBox(topicName + " - " + streamTypeToString(topic.type));

        connect(button, &QCheckBox::toggled, this, [this, topicName](const bool checked) {
            this->onTopicToggled(topicName, checked);
        });
        m_topicsLayout->addWidget(button, index / s_topicColumns, index % s_topicColumns);
        index++;
        m_topicButtonsByName.insert(topicName, button);
    }
}

void ClientWindow::onTopicToggled(const QString &topicName, bool checked)
{
    if (checked) {
        m_clientSession->subscribeToTopic(topicName.toStdString());
    } else {
        m_clientSession->unSubscribeFromTopic(topicName.toStdString());
        removeGraph(topicName);
    }
}

void ClientWindow::clearTopicButtons()
{
    for (QCheckBox *button : m_topicButtonsByName)
        button->deleteLater();
    
    m_topicButtonsByName.clear();
}

QString ClientWindow::streamTypeToString(StreamType type)
{
    switch (type)
    {
    case StreamType::FINANCE:
        return "FINANCE";
    case StreamType::WEATHER:
        return "WEATHER";
    case StreamType::SENSOR:
        return "SENSOR";
    default:
        return "UNKNOWN";
    };
}
