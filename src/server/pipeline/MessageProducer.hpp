//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_MESSAGEPRODUCER_HPP
#define FLOWBROKER_MESSAGEPRODUCER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include "MessageCatalog.hpp"
#include "../data_sources/IDataSource.hpp"
#include "../topics/TopicCache.hpp"

/**
 * @class MessageProducer
 * @brief Owns the data sources and feeds their output into the catalog.
 *
 * On start() it builds the sources, declares their topics in the cache (so they
 * are known before their first value), and starts them. Each source then
 * produces messages on the shared io_context's timer, and the producer pushes
 * every one into the MessageCatalog for the consumer to take.
 *
 * The producer has no thread of its own: the sources' timers run on whatever
 * thread drives the io_context (the server thread here).
 */
class MessageProducer {
public:
    MessageProducer(boost::asio::io_context& ioContext, MessageCatalog& messageCatalog, TopicCache& topicCache);
    
    /// Build the sources, declare their topics, and start producing.
    void start();
    
    /// Stop all sources.
    void stop() const;
private:
    /// Push one produced message into the catalog.
    void addMessage(const Message& message) const;

    void initDataSources();       ///< Create the data sources (currently simulated finance).
    void initTopics() const;      ///< Declare each source's topics in the cache.
    void startDataSources() const;///< Start every source, wiring its output to addMessage.
    void stopDataSources() const; ///< Stop every source.

    boost::asio::io_context& m_ioContext;    ///< Drives the simulated sources' timers.
    TopicCache& m_topicCache;
    MessageCatalog &m_messageCatalog;        ///< Holds the produced messages.
    std::vector<std::unique_ptr<IDataSource>> m_dataSources;
};


#endif //FLOWBROKER_MESSAGEPRODUCER_HPP
