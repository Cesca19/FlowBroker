//
// Created by fattindehou on 16/09/2026.
//

#include "UdpReceiver.hpp"

UdpReceiver::UdpReceiver(QObject *parent)
    : QObject(parent)
    , m_udpSocket(new QUdpSocket(this))
{
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);
}

void UdpReceiver::start(int udpPort)
{
    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState)
        m_udpSocket->close();
    if (m_udpSocket->bind(QHostAddress(QHostAddress::AnyIPv4), udpPort))
        emit updConnectionEstablished();
    else
        emit udpConnectionFailed(m_udpSocket->errorString());
}

void UdpReceiver::close()
{
    m_udpSocket->close();
}

void UdpReceiver::readPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        emit messageReceived(buffer.data());
        // std::cout << "Received datagram from " << sender.toString().toStdString() << ":" << senderPort << " - " << buffer.toStdString();
    }
}