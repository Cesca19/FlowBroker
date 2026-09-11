//
// Created by fran on 29/07/2026.
//

#ifndef FLOWBROKER_TCPCLIENTCONNECTION_HPP
#define FLOWBROKER_TCPCLIENTCONNECTION_HPP

#include <string>
#include <QTcpSocket>
#include <QApplication>
#include "UIUtils.hpp"

/**
 * @class TcpClientConnection
 * @brief Owns the TCP control socket and turns its byte stream into lines.
 *
 * This is the transport layer of the client.
 * It knows nothing about the protocol itself: it opens and closes the socket,
 * accumulates incoming bytes until a complete line is available, and emits that line.
 */
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
    void onDisconnected();

    /// Append the new bytes to the buffer and emit every complete line it holds.
    void onMessageReceived();
    void onConnectionError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

public:
    void sendMessage(const std::string &message);

private:
    QTcpSocket *m_tcpSocket;
    QByteArray m_buffer;
    QAbstractSocket::SocketState m_socketState;
};


#endif //FLOWBROKER_TCPCLIENTCONNECTION_HPP
