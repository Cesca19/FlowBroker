//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "QTcpClient.hpp"

int main(int ac, char **av)
{
    if (ac < 3) {
        std::cerr << "Usage: " << av[0] << " <host> <port>" << std::endl;
        return 1;
    }
    int port = -1;
    try {
        port = std::stoi(av[2]);
    } catch (std::exception &e) {
        std::cout << "Error: String '" << av[2] << "' is not a valid number\n";
        return 1;
    }
    QApplication app(ac, av);
    QTcpClient tcpClient(av[1], port);

    tcpClient.show();
    return app.exec();
}
