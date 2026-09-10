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
        // message arch: TYPE;name;ts;value;average;min;max
        const std::string topicMessage = formatTopicSnapshot(snapshot);
        messageToSend += topicMessage;
    }
    messageToSend.pop_back(); // remove the last newline character
    // m_tcpServer.sendMessageToAllClients(messageToSend);
    m_dashBoardRefreshTimer.expires_at(m_dashBoardRefreshTimer.expiry() + m_refreshTime);
    m_dashBoardRefreshTimer.async_wait(std::bind(&Server::refreshTopicsDashBoard, this, std::placeholders::_1));
}

std::string Server::formatTopicSnapshot(const TopicSnapshot &snapshot) const
{
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
