//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "pipeline/MessageConsumer.hpp"
#include "pipeline/Server.hpp"

int main()
{
    std::cout << "Hello from the server !" << std::endl;
    boost::asio::io_context ioContext;
    MessageCatalog messageCatalog;
    Server server(ioContext, messageCatalog);
    MessageConsumer messageConsumer(messageCatalog);

    std::thread m_consumerThread(&MessageConsumer::run, &messageConsumer);
    std::thread m_serverThread(&Server::run, &server);

    if (m_serverThread.joinable())
        m_serverThread.join();
    if (m_consumerThread.joinable())
        m_consumerThread.join();
    return 0;
}