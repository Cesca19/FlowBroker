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
#include "TopicGraph.hpp"
#include "NetworkUtils.hpp"
#include "ClientSession.hpp"

/**
 * @class ClientWindow
 * @brief Main window: It holds the connection form, and one live chart per topic.
 *
 * Top of the client. It owns the ClientSession and is the only class that
 * touches widgets: it validates what the user typed before any connection is
 * attempted, reflects the connection state on the form and the button, and
 * creates a TopicGraph the first time a value arrives for a topic.
 *
 * The command line arguments only pre-fill the form; nothing is validated or
 * connected until the user clicks Connect. Charts are held in a scroll area, so
 * subscribing to many topics stays usable. Reaching the connected state
 * triggers the HELLO that opens the session.
 */
class ClientWindow : public QWidget {
    Q_OBJECT
public:
    explicit ClientWindow(const std::string &host, int tcpPort, int udpPort, QWidget *parent = nullptr);
private:

signals:
private slots:
    void onAddMessageRequested(const QString &messageTitle, const QString &messageContent, MessageType messageType);

    /// Validate the form, then connect or disconnect depending on the state.
    void onConnectButtonClicked();
    void onNewTopicReceived(const QString &topicName, const qint64 tsMs, const double value);

    /// Update the form and the button, and react to the edges of the state.
    void onTcpConnectionStateChanged(ConnectionState connectionState);
private:
    TopicGraph* findOrCreateGraph(const QString& topicName);
    void clearGraphs();
    void onTcpClientConnected() const;

    int m_tcpPort;
    int m_udpPort;
    std::string m_host;
    ConnectionState m_tcpConnectionState;
    ClientSession *m_clientSession;
    QLineEdit *m_hostEdit;
    QLineEdit *m_tcpPortEdit;
    QLineEdit *m_udpPortEdit;
    QPushButton *m_connectBtn;
    QVBoxLayout* m_graphsLayout{};
    QHash<QString, TopicGraph*> m_graphsByTopic;
};


#endif //FLOWBROKER_CLIENTWINDOW_HPP
