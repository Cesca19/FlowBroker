//
// Created by fran on 06/07/2026.
//

/**
 * @file main.cpp
 * @brief Entry point of the FlowBroker server.
 *
 * Parses the command line, then builds and runs
 * the server. FlowBroker ingests real-time data streams and routes them, by
 * topic, to subscribed clients. A "topic" is a named stream such as a stock
 * price or a weather reading.
 *
 * Here is the path of a message through the server:
 *   - Data sources (IDataSource, e.g. SimulatedFinanceDataSource) produce the
 *     raw values of one or more topics.
 *   - The producer (MessageProducer) starts the sources and pushes their
 *     messages into the catalog.
 *   - The catalog (MessageCatalog) is the thread-safe queue linking producer
 *     and consumer.
 *   - The consumer (MessageConsumer) drains the catalog and updates the cache.
 *   - The cache (TopicCache) holds the latest value and stats of every topic.
 *   - The server (Server, which owns TcpServer and per-client TcpConnection)
 *     accepts clients and serves them topic data.
 *
 * Threads (only two are started here; the main itself waits):
 *   - Server thread: runs Server::run, which starts the producer and then
 *     drives the single Asio io_context. Everything asynchronous happens here:
 *     the data source timers that produce messages, accepting TCP clients,
 *     sending, and the dashboard refresh timer. The producer has no thread of
 *     its own; its work is driven by those timers on this thread.
 *   - Consumer thread: runs MessageConsumer::run, a blocking loop that pops
 *     from the catalog and writes to the cache.
 *   - Main thread: builds the pieces, launches the two threads, and blocks on
 *     join until they finish.
 *
 * MessageCatalog is the hand-off point between the two threads: the producer
 * side (server thread) fills it, the consumer thread drains it. It encapsulates
 * its own mutex and condition variable, so callers never lock anything
 * themselves to use it safely.
 */

#include <iostream>
#include "pipeline/MessageConsumer.hpp"
#include "network/Server.hpp"

static void printUsage(std::ostream &out, const std::string &programName)
{
    out << "Usage: " << programName << " [tcp_port]" << std::endl
        << std::endl
        << "  tcp_port    TCP control port to listen on (default "
        << network::defaultTcpPort << ")." << std::endl
        << "              Must be in the range " << network::minPort << ".."
        << network::maxPort << "." << std::endl
        << "  -h, --help  Show this message and exit." << std::endl
        << "\n"
        << "The UDP data socket is send-only and uses an ephemeral port chosen" << std::endl
        << "by the OS, so it needs no configuration." << std::endl;
}

static int launchServer(int tcpPort)
{
    try {
        boost::asio::io_context ioContext;
        MessageCatalog messageCatalog;
        TopicCache topicCache;

        Server server(ioContext, messageCatalog, topicCache, tcpPort);
        MessageConsumer messageConsumer(messageCatalog, topicCache);

        std::thread consumerThread(&MessageConsumer::run, &messageConsumer);
        std::thread serverThread(&Server::run, &server);

        if (serverThread.joinable())
            serverThread.join();
        if (consumerThread.joinable())
            consumerThread.join();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
}

int main(int ac, char **av)
{
    if (ac == 1)
        return launchServer(network::defaultTcpPort);

    if (ac > 2) {
        std::cerr << "Error: too many arguments.\n\n";
        printUsage(std::cerr, av[0]);
        return 84;
    }

    const std::string arg1(av[1]);
    if (arg1 == "-h" || arg1 == "--help") {
        printUsage(std::cout, av[0]);
        return 0;
    }
    const std::optional<std::uint16_t> tcpPort = parsePort(arg1);
    if (!tcpPort) {
        std::cerr << "Error: invalid port \"" << arg1 << "\" (expected an integer in " << network::minPort << ".." << network::maxPort << " range)." << std::endl << std::endl;
        printUsage(std::cerr, av[0]);
        return 84;
    }
    return launchServer(*tcpPort);
}