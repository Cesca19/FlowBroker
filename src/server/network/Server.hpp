//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_SERVER_HPP
#define FLOWBROKER_SERVER_HPP

#include "TcpServer.hpp"
#include "UdpSender.hpp"
#include "../topics/TopicCache.hpp"
#include "../pipeline/MessageProducer.hpp"

/**
 * @class Server
 * @brief Manages the whole server and drives the io_context.
 *
 * Owns the message producer, the TCP server, the shutdown signal handler and the
 * dashboard refresh timer, all sharing one io_context.
 */
class Server {
public:
    Server(boost::asio::io_context &ioContext, MessageCatalog &catalog, TopicCache &topicCache, int tcpPort);
    
    /// Start the producer and timers, then run the io_context.
    void run();
    void stop();
private:
    void refreshTopicsDashBoard(const boost::system::error_code& error);

    boost::asio::io_context &m_ioContext;
    boost::asio::signal_set m_signals;
    MessageCatalog &m_messageCatalog;
    TopicCache &m_topicCache;
    MessageProducer m_messageProducer;
    boost::asio::steady_timer m_dashBoardRefreshTimer;
    std::chrono::seconds m_refreshTime;
    TcpServer m_tcpServer;
};


#endif //FLOWBROKER_SERVER_HPP
