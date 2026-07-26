//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_SERVER_HPP
#define FLOWBROKER_SERVER_HPP

#include "MessageProducer.hpp"

class Server {
public:
    Server(boost::asio::io_context &ioContext, MessageCatalog &catalog);
    void run();
    void stop();
private:
    boost::asio::io_context &m_ioContext;
    boost::asio::signal_set m_signals;
    MessageCatalog &m_messageCatalog;
    MessageProducer m_messageProducer;
};


#endif //FLOWBROKER_SERVER_HPP
