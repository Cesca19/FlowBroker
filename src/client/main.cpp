//
// Created by fran on 06/07/2026.
//

/**
 * @file main.cpp
 * @brief Entry point of the FlowBroker client.
 *
 * Parses the command line, then opens the main window. The client subscribes to
 * the topics it cares about and plots the values the server pushes back.
 *
 * Here is the path of a message through the client:
 *   - The transport (TcpClientConnection) owns the control socket and 
 *      receives the incoming messages.
 *   - The session (ClientSession) implements the control protocol: it formats the
 *     commands going out, parses the lines coming in, and re-emits them as
 *     typed signals.
 *   - The window (ClientWindow) validates the connection settings and reflects
 *     the connection state in the form.
 *   - The charts (TopicGraph, one per topic, created on demand) plot the values
 *     against time.
 *
 * Everything runs on the Qt event loop of the main thread. The
 * socket is asynchronous, so waiting on the network never blocks the interface,
 * and no shared state needs protecting anywhere in the client.
 * 
 * The arguments are optional and only pre-fill the connection form: an invalid
 * port is silently ignored here and simply leaves its default in place.
 */

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
