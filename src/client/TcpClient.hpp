//
// Created by fran on 29/07/2026.
//

#ifndef FLOWBROKER_TCPCLIENT_HPP
#define FLOWBROKER_TCPCLIENT_HPP

#include <string>
#include <QTcpSocket>
#include <QPushButton>
#include <QMessageBox>
#include <QHash>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QApplication>
#include "TopicGraph.hpp"

class TcpClient  : public QWidget {
    Q_OBJECT
public:
    explicit TcpClient(const std::string &host, int port, QWidget *parent = nullptr);
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
    void handleServerMessage(const QString &message);
    void onNewTopicSnapshotReceived(const QStringList &message);
    void onConnectionError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    TopicGraph* findOrCreateGraph(const QString& topicName);
    void clearGraphs();

    int m_port;
    std::string m_host;
    QPushButton *m_connectBtn;
    QTcpSocket *m_tcpSocket;
    QByteArray m_buffer;
    QAbstractSocket::SocketState m_socketState;
    QVBoxLayout* m_graphsLayout{};
    QHash<QString, TopicGraph*> m_graphsByTopic;
};


#endif //FLOWBROKER_TCPCLIENT_HPP
