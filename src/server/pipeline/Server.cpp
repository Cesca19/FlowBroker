//
// Created by fran on 25/07/2026.
//

#include "Server.hpp"

Server::Server(boost::asio::io_context &ioContext, MessageCatalog &catalog)
    : m_ioContext(ioContext)
    , m_signals(ioContext, SIGINT)
    , m_messageCatalog(catalog)
    , m_messageProducer(ioContext, m_messageCatalog)
{
}

void Server::run()
{
    m_messageProducer.start();
    m_signals.async_wait([this](const boost::system::error_code&, int) {
        stop();   // Ctrl+C pressed
    });
    m_ioContext.run();
}

void Server::stop()
{
    m_messageProducer.stop();
    m_ioContext.stop();
    m_messageCatalog.stop();
}
