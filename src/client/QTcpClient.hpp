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
#include <queue>

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
    void onConnectButtonClicked(bool checked) const;
    void onConnected();
    void onDisconnected();
    void onMessageReceived();
    void onConnectionError(QAbstractSocket::SocketError socketError);

private:
    int m_port;
    std::string m_host;
    bool m_isConnected;
    std::unique_ptr<QPushButton> m_connectBtn;
    std::unique_ptr<QTcpSocket> m_tcpSocket;
    QByteArray m_buffer;
};


#endif //FLOWBROKER_QTCPCLIENT_HPP
