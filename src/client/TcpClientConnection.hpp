//
// Created by fran on 29/07/2026.
//

#ifndef FLOWBROKER_TCPCLIENTCONNECTION_HPP
#define FLOWBROKER_TCPCLIENTCONNECTION_HPP

#include <string>
#include <QTcpSocket>
#include <QApplication>
#include "UIUtils.hpp"

class TcpClientConnection  : public QObject {
    Q_OBJECT
public:
    explicit TcpClientConnection(QObject *parent = nullptr);
    void connectToServer(const std::string &host, std::uint16_t port) const;
    void disconnectFromServer() const;
private:
    void initConnection();
signals:
    void connectionStateChanged(ConnectionState connectionState);
    void addMessage(const QString &messageTitle, const QString &messageContent, MessageType messageType = MessageType::Info);
    void messageReceived(const QString &messageContent);
private slots:
    void onConnected();
    void onDisconnected() const;
    void onMessageReceived();
    void onConnectionError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    void sendMessage(const std::string &message);

    QTcpSocket *m_tcpSocket;
    QByteArray m_buffer;
    QAbstractSocket::SocketState m_socketState;
};


#endif //FLOWBROKER_TCPCLIENTCONNECTION_HPP
