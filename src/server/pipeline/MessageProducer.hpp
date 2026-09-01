//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_MESSAGEPRODUCER_HPP
#define FLOWBROKER_MESSAGEPRODUCER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include "MessageCatalog.hpp"
#include "../data_sources/IDataSource.hpp"

class MessageProducer {
public:
    MessageProducer(boost::asio::io_context& ioContext, MessageCatalog& messageCatalog);
    void start();
    void stop() const;
private:
    void addMessage(const Message& message) const;
    void initDataSources();
    void startDataSources() const;
    void stopDataSources() const;

    boost::asio::io_context& m_ioContext;
    MessageCatalog &m_messageCatalog;

    std::vector<std::unique_ptr<IDataSource>> m_dataSources;
};


#endif //FLOWBROKER_MESSAGEPRODUCER_HPP
