//
// Created by fran on 01/09/2026.
//

#ifndef FLOWBROKER_CLIENTWINDOW_HPP
#define FLOWBROKER_CLIENTWINDOW_HPP

#include <QWidget>
#include <QHash>
#include <QLineEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMessageBox>
#include "TcpClient.hpp"
#include "TopicGraph.hpp"
#include "NetworkUtils.hpp"


class ClientWindow : public QWidget {
    Q_OBJECT
public:
    explicit ClientWindow(const std::string &host, int tcpPort, int udpPort, QWidget *parent = nullptr);
private:

signals:
private slots:
    void onAddMessageRequested(const QString &messageTitle, const QString &messageContent, MessageType messageType);
    void onConnectButtonClicked();
    void onNewTopicReceived(const QString &topicName, const qint64 tsMs, const double value);
    void onTcpConnectionStateChanged(ConnectionState connectionState);
private:
    TopicGraph* findOrCreateGraph(const QString& topicName);
    void clearGraphs();

    int m_tcpPort;
    int m_udpPort;
    std::string m_host;
    ConnectionState m_tcpConnectionState;
    TcpClient *m_tcpClient;
    QLineEdit *m_hostEdit;
    QLineEdit *m_tcpPortEdit;
    QLineEdit *m_udpPortEdit;
    QPushButton *m_connectBtn;
    QVBoxLayout* m_graphsLayout{};
    QHash<QString, TopicGraph*> m_graphsByTopic;
};


#endif //FLOWBROKER_CLIENTWINDOW_HPP
