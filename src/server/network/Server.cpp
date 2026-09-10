//
// Created by fran on 25/07/2026.
//

#include "Server.hpp"

Server::Server(boost::asio::io_context &ioContext, MessageCatalog &catalog, TopicCache &topicCache, const int tcpPort)
    : m_ioContext(ioContext)
    , m_signals(ioContext, SIGINT)
    , m_messageCatalog(catalog)
    , m_topicCache(topicCache)
    , m_messageProducer(ioContext, m_messageCatalog, topicCache)
    , m_dashBoardRefreshTimer(ioContext)
    , m_refreshTime(1)
    , m_tcpServer(ioContext, tcpPort, topicCache)
    , m_udpSender(ioContext, 0)  // Use port 0 to let the OS choose an available port
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
    if (error == boost::asio::error::operation_aborted) {
        // Timer was canceled, likely due to server shutdown. No action needed.
        return;
    }
    if (error) {
        std::cerr << "Error in timer refresh: " << error.message() << std::endl;
        return;
    }

    const std::vector<TopicSnapshot> topicSnapshots = m_topicCache.getAllTopicsSnapshot();

    std::string messageToSend;
    for (const auto& snapshot : topicSnapshots) {
        sendTopicDataToClients(snapshot);
        
    }
    // m_tcpServer.sendMessageToAllClients(messageToSend);
    m_dashBoardRefreshTimer.expires_at(m_dashBoardRefreshTimer.expiry() + m_refreshTime);
    m_dashBoardRefreshTimer.async_wait(std::bind(&Server::refreshTopicsDashBoard, this, std::placeholders::_1));
}

void Server::sendTopicDataToClients(const TopicSnapshot &snapshot)
{
    const std::string topicMessage = formatTopicSnapshot(snapshot);

    const std::vector<boost::asio::ip::udp::endpoint> endpoints = m_tcpServer.getUdpEndpointsForTopic(snapshot.topicName);
    if (!endpoints.empty()) {
        auto dataToSend = std::make_shared<std::string>(topicMessage);
        m_udpSender.sendTo(endpoints, dataToSend);
    }
}

std::string Server::formatTopicSnapshot(const TopicSnapshot &snapshot) const
{
    // message format: "TOPIC;topicName;timestampNs;lastValuesByField;averagesValuesByField;minValuesByField;maxValuesByField\n"
    const std::string topicMessage = "TOPIC;" + snapshot.topicName + ";" + std::to_string(snapshot.timestampNs) + ";" 
                                    + formatVectorOfDoubles(snapshot.lastValuesByField, ':') + ";" 
                                    + formatVectorOfDoubles(snapshot.averagesValuesByField, ':') + ";" 
                                    + formatVectorOfDoubles(snapshot.minValuesByField, ':') + ";"
                                    + formatVectorOfDoubles(snapshot.maxValuesByField, ':') + "\n";
    return topicMessage;
}

std::string Server::formatVectorOfDoubles(const std::vector<double> &values, char delimiter) const
{
    std::string formattedString;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            formattedString += delimiter;
        }
        formattedString += std::to_string(values[i]);
    }
    return formattedString;
}
