//
// Created by fran on 25/07/2026.
//

#include "Server.hpp"
#include <iostream>

Server::Server(boost::asio::io_context &ioContext, MessageCatalog &catalog, TopicCache &topicCache, const int tcpPort)
    : m_ioContext(ioContext)
    , m_signals(ioContext, SIGINT)
    , m_messageCatalog(catalog)
    , m_topicCache(topicCache)
    , m_messageProducer(ioContext, m_messageCatalog)
    , m_dashBoardRefreshTimer(ioContext)
    , m_refreshTime(1)
    , m_tcpServer(ioContext, tcpPort)
{
}

void Server::run()
{
    m_messageProducer.start();
    m_signals.async_wait([this](const boost::system::error_code&, int) {
        stop();   // Ctrl+C pressed
    });
    m_dashBoardRefreshTimer.expires_after(m_refreshTime);
    m_dashBoardRefreshTimer.async_wait(std::bind(&Server::refreshTopicsDashBoard, this, std::placeholders::_1));
    m_tcpServer.run();
    m_ioContext.run();
}

void Server::stop()
{
    m_messageProducer.stop();
    m_dashBoardRefreshTimer.cancel();
    m_ioContext.stop();
    m_messageCatalog.stop();
}

void Server::refreshTopicsDashBoard(const boost::system::error_code &error)
{
    if (error) {
        std::cerr << "Error in timer refresh: " << error.message() << std::endl;
        return;
    }

    const std::vector<TopicSnapshot> topicSnapshots = m_topicCache.getAllTopicsSnapshot();

    std::string messageToSend;
    for (const auto&[topicName, timeStampNs, lastValue, average, min, max] : topicSnapshots) {
        // message arch: TYPE;name;ts;value;average;min;max
        const std::string topicMessage = "TOPIC:" + topicName + ":" + std::to_string(timeStampNs) + ":" + std::to_string(lastValue) + ":" +
            std::to_string(average) + ":" + std::to_string(min) + ":" + std::to_string(max) + "\n";
        std::cout << topicMessage;
        messageToSend += topicMessage;
    }
    m_tcpServer.sendMessageToAllClients(messageToSend);
    m_dashBoardRefreshTimer.expires_at(m_dashBoardRefreshTimer.expiry() + m_refreshTime);
    m_dashBoardRefreshTimer.async_wait(std::bind(&Server::refreshTopicsDashBoard, this, std::placeholders::_1));
}
