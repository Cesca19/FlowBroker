//
// Created by fran on 02/09/2026.
//

#ifndef FLOWBROKER_CLIENTSESSION_HPP
#define FLOWBROKER_CLIENTSESSION_HPP

#include "TcpClientConnection.hpp"

/**
 * @class ClientSession
 * @brief Implements the FlowBroker control protocol on top of the TCP transport.
 *
 * It holds an instance of `TcpClientConnection` and sits between it and the user interface: 
 *  - it formats the messages to be sent to the server (HELLO, TOPICS, SUB, UNSUB, ALERT, BYE),
 *  - and parses the incoming messages and emits signals to the UI.
 */
class ClientSession : public QObject {
    Q_OBJECT
public:
    explicit ClientSession(QObject *parent = nullptr);
    void connectTcpClient(const std::string &host, std::uint16_t port) const;
    void disconnectTcpClient() const;
signals:
    void tcpConnectionStateChanged(ConnectionState connectionState);
    /// One data point of a topic, ready to be plotted.
    void newTopicReceived(const QString &topicName, qint64 tsMs, double value);
    void addMessage(const QString &messageTitle, const QString &messageContent, MessageType messageType = MessageType::Info);
private slots:
    /// Identify one received line and route it to its parsing function.
    void handleTcpServerMessage(const QString &messageContent);

public slots:
    void sendHello(int udpPort) const;
    void getTopics() const;
    void subscribeToTopic(const std::string &topicName) const;
    void unSubscribeFromTopic(const std::string &topicName) const;
    void createAlert(const std::string &topicName, const std::string &topicField, const std::string &op,
        double value) const;
    void sendBye() const;
private:
    void onNewTopicSnapshotReceived(const QStringList &message);

    TcpClientConnection *m_tcpConnection;
};


#endif //FLOWBROKER_CLIENTSESSION_HPP
