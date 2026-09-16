//
// Created by fattindehou on 16/09/2026.
//

#ifndef FLOWBROKER_UDPRECEIVER_HPP
#define FLOWBROKER_UDPRECEIVER_HPP

#include <QObject>
#include <QUdpSocket>

class UdpReceiver : public QObject {
    Q_OBJECT
public:
    explicit UdpReceiver(QObject *parent = nullptr);
    void start(int udpPort);
    void close();
signals:
    void updConnectionEstablished();
    void udpConnectionFailed(const QString &errorMessage);
    void messageReceived(const QString &messageContent);
private slots:
    void readPendingDatagrams();
private:
    QUdpSocket *m_udpSocket;
};


#endif //FLOWBROKER_UDPRECEIVER_HPP