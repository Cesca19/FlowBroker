//
// Created by fran on 29/07/2026.
//

#ifndef FLOWBROKER_QTCPCLIENT_HPP
#define FLOWBROKER_QTCPCLIENT_HPP

#include <string>
#include <QTcpSocket>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>

class QTcpClient  : public QWidget {
    Q_OBJECT
public:
    explicit QTcpClient(const std::string &host, int port, QWidget *parent = nullptr);
private:
    void initConnection();
    void connectToServer() const;
    void disconnectFromServer() const;
signals:
    void quit();
private slots:
    void onConnectButtonClicked() const;
    void onConnected() const;
    void onDisconnected() const;
    void onMessageReceived();
    void onConnectionError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    int m_port;
    std::string m_host;
    std::unique_ptr<QPushButton> m_connectBtn;
    std::unique_ptr<QTcpSocket> m_tcpSocket;
    QByteArray m_buffer;
    QAbstractSocket::SocketState m_socketState;
};


#endif //FLOWBROKER_QTCPCLIENT_HPP
