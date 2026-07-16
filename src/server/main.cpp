//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include "GeometricBrownianMotion.hpp"
#include "SimulatedFinancialAsset.hpp"

void testAssetSimulation()
{
    // 1. Configure ONE asset.
    FinancialAssetConfig config{"GOLD", 180.0, 0.0001, 0.02};
    SimulatedFinancialAsset asset(config);
 
    // 2. Drive it over time: call getNextPrice() N times, one call = one tick.
    //    Store each (time, price) so we can inspect and plot the trajectory.
    const int nbTicks = 1000;
    std::vector<double> times;
    std::vector<double> prices;
    times.reserve(nbTicks);
    prices.reserve(nbTicks);
 
    for (int t = 0; t < nbTicks; t++) {
        double price = asset.getNextPrice(); // advance one tick over time
        times.push_back(t);
        prices.push_back(price);
    }
 
    // 3. Invariant checks over the whole trajectory.
    std::cout << "ticks: " << prices.size() << " (expected " << nbTicks << ")\n";
 
    bool allPositive = std::all_of(prices.begin(), prices.end(),
                                   [](double v) { return v > 0.0; });
    std::cout << "all prices > 0: " << (allPositive ? "yes" : "NO") << "\n";
 
    // After the first tick the price should still be close to the start value.
    std::cout << "first tick price: " << prices.front()
              << " (started at " << config.initialPrice << ")\n";
 
    // getCurrentPrice() must equal the last value returned by getNextPrice().
    std::cout << "current == last price: "
              << (asset.getCurrentPrice() == prices.back() ? "yes" : "NO") << "\n";
 
    auto [mn, mx] = std::minmax_element(prices.begin(), prices.end());
    std::cout << "min: " << *mn << "  max: " << *mx << "\n";
 
    std::cout << "first 5 ticks: ";
    for (int i = 0; i < 5; ++i) std::cout << prices[i] << " ";
    std::cout << "\n";
 
    // 4. Write the trajectory as CSV to plot it later.
    std::ofstream out("asset_output.csv");
    out << "time,price\n";
    for (std::size_t i = 0; i < prices.size(); ++i)
        out << times[i] << "," << prices[i] << "\n";
    std::cout << "CSV written to asset_output.csv\n";
}

int main()
{
    std::cout << "Hello from the server !" << std::endl;
    GBM::testGMBSimulation();
    testAssetSimulation();
    return 0;
}