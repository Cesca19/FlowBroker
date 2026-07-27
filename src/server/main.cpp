//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "pipeline/MessageConsumer.hpp"
#include "network/Server.hpp"

int main()
{
    std::cout << "Hello from the server !" << std::endl;
    boost::asio::io_context ioContext;
    MessageCatalog messageCatalog;
    TopicCache topicCache;

    Server server(ioContext, messageCatalog, topicCache);
    MessageConsumer messageConsumer(messageCatalog, topicCache);

    std::thread m_consumerThread(&MessageConsumer::run, &messageConsumer);
    std::thread m_serverThread(&Server::run, &server);

    if (m_serverThread.joinable())
        m_serverThread.join();
    if (m_consumerThread.joinable())
        m_consumerThread.join();
    return 0;
}