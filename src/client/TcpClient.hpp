//
// Created by fran on 29/07/2026.
//

#ifndef FLOWBROKER_TCPCLIENT_HPP
#define FLOWBROKER_TCPCLIENT_HPP

#include <string>
#include <QTcpSocket>
#include <QApplication>
#include "UIUtils.hpp"

class TcpClient  : public QObject {
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToServer(const std::string &host, std::uint16_t port) const;
    void disconnectFromServer() const;
private:
    void initConnection();
signals:
    void connectionStateChanged(ConnectionState connectionState);
    void newTopicReceived(const QString &topicName, qint64 tsMs, double value);
    void addMessage(const QString &messageTitle, const QString &messageContent, MessageType messageType = MessageType::Info);
private slots:
    void onConnected() const;
    void onDisconnected() const;
    void onMessageReceived();
    void handleServerMessage(const QString &message);
    void onNewTopicSnapshotReceived(const QStringList &message);
    void onConnectionError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    QTcpSocket *m_tcpSocket;
    QByteArray m_buffer;
    QAbstractSocket::SocketState m_socketState;
};


#endif //FLOWBROKER_TCPCLIENT_HPP
