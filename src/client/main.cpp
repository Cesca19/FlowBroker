//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "ClientWindow.hpp"

static void printUsage(std::ostream &out, const std::string &programName)
{
    out << "Usage: " << programName << " [host [tcp_port [udp_port]]]" << std::endl
        << std::endl
        << "  host        Server IP address to connect to (default "
        << network::defaultHost << ")." << std::endl
        << "  tcp_port    Server TCP control port (default "
        << network::defaultTcpPort << ", range " << network::minPort << ".."
        << network::maxPort << ")." << std::endl
        << "  udp_port    Local UDP port to receive the data stream on (default "
        << network::defaultUdpPort << ", range " << network::minPort << ".."
        << network::maxPort << ")." << std::endl
        << "  -h, --help  Show this message and exit." << std::endl
        << std::endl
        << "All arguments are optional and only pre-fill the connection form."
        << std::endl
        << "Nothing is validated or connected until you click Connect." << std::endl;
}

int main(int ac, char **av)
{
    std::string host = std::string(network::defaultHost);
    std::uint16_t tcpPort = network::defaultTcpPort;
    std::uint16_t udpPort = network::defaultUdpPort;

    if (ac >= 2 && (std::string_view(av[1]) == "-h" || std::string_view(av[1]) == "--help")) {
        printUsage(std::cout, av[0]);
        return 0;
    }

    if (ac >= 2)
        host = av[1];
    if (ac >= 3)
        if (const auto port = parsePort(av[2]))
            tcpPort = *port;
    if (ac >= 4)
        if (const auto port = parsePort(av[3]))
            udpPort = *port;

    QApplication app(ac, av);
    ClientWindow clientWindow(host, tcpPort, udpPort);
    clientWindow.show();
    return app.exec();
}
