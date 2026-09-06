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

class MessageProducer {
public:
    MessageProducer(boost::asio::io_context& ioContext, MessageCatalog& messageCatalog, TopicCache& topicCache);
    void start();
    void stop() const;
private:
    void addMessage(const Message& message) const;
    void initDataSources();
    void initTopics() const;
    void startDataSources() const;
    void stopDataSources() const;

    boost::asio::io_context& m_ioContext;
    TopicCache& m_topicCache;
    MessageCatalog &m_messageCatalog;
    std::vector<std::unique_ptr<IDataSource>> m_dataSources;
};


#endif //FLOWBROKER_MESSAGEPRODUCER_HPP
