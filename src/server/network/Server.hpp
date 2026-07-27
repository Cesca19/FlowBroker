//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_SERVER_HPP
#define FLOWBROKER_SERVER_HPP

#include "../topics/TopicCache.hpp"
#include "../pipeline/MessageProducer.hpp"

class Server {
public:
    Server(boost::asio::io_context &ioContext, MessageCatalog &catalog, TopicCache &topicCache);
    void run();
    void stop();
private:
    void refreshTopicsDashBoard(const boost::system::error_code& error);

    boost::asio::io_context &m_ioContext;
    boost::asio::signal_set m_signals;
    MessageCatalog &m_messageCatalog;
    TopicCache &m_topicCache;
    MessageProducer m_messageProducer;

    // debug
    boost::asio::steady_timer m_dashBoardRefreshTimer;
    std::chrono::seconds m_refreshTime;
};


#endif //FLOWBROKER_SERVER_HPP
