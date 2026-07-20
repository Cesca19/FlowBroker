//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include "SimulatedFinanceDataSource.hpp"

std::string make_daytime_string()
{
    std::time_t now = std::time(0);
    
    return std::ctime(&now);
}

void tcp_server(boost::asio::io_context& io_context, unsigned int port)
{
    try {
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

        for (;;) {
            boost::asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::string message = make_daytime_string();
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
            std::cout << "Sent message: " << message << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main()
{
    std::cout << "Hello from the server !" << std::endl;

    boost::asio::io_context io;
    tcp_server(io, 13);
    return 0;

    // FinancialAssetConfig config{"GOLD", 180.0, 0.0001, 0.02};
    // SimulatedFinanceDataSource source(io, boost::asio::chrono::milliseconds(20), config);

    // std::vector<Message> collected;
    // source.start([&](const Message& m) {
    //     collected.push_back(m);
    // });
    // io.run_for(std::chrono::seconds(5));
    // source.stop();

    // std::cout << "collected " << collected.size() << " messages\n";
    // std::ofstream out("finance_output.csv");
    // out << "timestamp_ns,topic,price\n";
    // for (const auto& m : collected)
    //     out << m.timestampNs << "," << m.topic << "," << m.values.at(0) << "\n";
    // std::cout << "CSV written to finance_output.csv\n";
    return 0;
}