//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include "SimulatedFinanceDataSource.hpp"

int main()
{
    std::cout << "Hello from the server !" << std::endl;

    boost::asio::io_context io;
    FinancialAssetConfig config{.name = "GOLD", .initialPrice = 180.0, .drift = 0.0001, .volatility = 0.02};
    SimulatedFinanceDataSource source(io, boost::asio::chrono::milliseconds(200), config);

    std::vector<Message> collected;
    source.start([&](const Message& m) {
        collected.push_back(m);
    });
    io.run_for(std::chrono::seconds(5));
    source.stop();

    std::cout << "collected " << collected.size() << " messages\n";
    std::ofstream out("finance_output.csv");
    out << "timestamp_ns,topic,price\n";
    for (const auto& m : collected)
        out << m.timestampNs << "," << m.topic << "," << m.values.at(0) << "\n";
    std::cout << "CSV written to finance_output.csv\n";
    return 0;
}