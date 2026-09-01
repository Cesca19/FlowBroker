//
// Created by fran on 06/07/2026.
//

#include <charconv>
#include <iostream>
#include "pipeline/MessageConsumer.hpp"
#include "network/Server.hpp"
#include "../common/NetworkDefaults.hpp"

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

static std::optional<std::uint16_t> parsePort(const std::string_view text)
{
    int value = 0;
    const char *begin = text.data();
    const char *end = text.data() + text.size();

    auto [ptr, ec] = std::from_chars(begin, end, value);
    // Reject if parsing failed, or if characters remain after the number.
    if (ec != std::errc() || ptr != end)
        return std::nullopt;
    if (value < network::minPort || value > network::maxPort)
        return std::nullopt;
    return static_cast<std::uint16_t>(value);
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