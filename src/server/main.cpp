//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "pipeline/MessageConsumer.hpp"
#include "network/Server.hpp"

int main(int ac, char **av)
{
    if(ac != 2) {
        std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
        return 1;
    }
    std::cout << "Hello from the server !" << std::endl;

    try {
        int tcpPort = std::stoi(av[1]);
        boost::asio::io_context ioContext;
        MessageCatalog messageCatalog;
        TopicCache topicCache;

        Server server(ioContext, messageCatalog, topicCache, tcpPort);
        MessageConsumer messageConsumer(messageCatalog, topicCache);

        std::thread m_consumerThread(&MessageConsumer::run, &messageConsumer);
        std::thread m_serverThread(&Server::run, &server);

        if (m_serverThread.joinable())
            m_serverThread.join();
        if (m_consumerThread.joinable())
            m_consumerThread.join();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}