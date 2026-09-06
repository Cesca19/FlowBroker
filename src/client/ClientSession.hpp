//
// Created by fran on 02/09/2026.
//

#ifndef FLOWBROKER_CLIENTSESSION_HPP
#define FLOWBROKER_CLIENTSESSION_HPP

#include "TcpClientConnection.hpp"


class ClientSession : public QObject {
    Q_OBJECT
public:
    explicit ClientSession(QObject *parent = nullptr);
    void connectTcpClient(const std::string &host, std::uint16_t port) const;
    void disconnectTcpClient() const;
signals:
    void tcpConnectionStateChanged(ConnectionState connectionState);
    void newTopicReceived(const QString &topicName, qint64 tsMs, double value);
    void addMessage(const QString &messageTitle, const QString &messageContent, MessageType messageType = MessageType::Info);
private slots:
    void handleTcpServerMessage(const QString &messageContent);

public slots:
    void sendHello(int m_udpPort) const;
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
