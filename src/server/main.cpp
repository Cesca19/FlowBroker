//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include "GeometricBrownianMotion.hpp"

int main()
{
    std::cout << "Hello from the server !" << std::endl;

    const int    nbSteps      = 1000;
    const double timeInterval = 1.0;
    const double drift        = 0.0001;
    const double volatility   = 0.02;
    const double startPrice    = 180.0;
    auto gaussianFunc =  GBM::gaussianFunctionGenerator();
    GBM::GBMSimulation sim = GBM::simulateGeometricBrownianMotion(
        nbSteps, timeInterval, drift, volatility, startPrice, gaussianFunc);


    // 1. Sizes match and equal nbSteps.
    std::cout << "steps: " << sim.simulatedValues.size()
              << " (expected " << nbSteps << ")\n";
    // 2. Every price is strictly positive. This is THE property exp() guarantees;
    //    if any value is <= 0, the model is broken.
    bool allPositive = std::all_of(
        sim.simulatedValues.begin(), sim.simulatedValues.end(),
        [](double v) { return v > 0.0; });
    std::cout << "all values > 0: " << (allPositive ? "yes" : "NO") << "\n";

    // 3. The path starts near exp(0) = 1 * initial price
    std::cout << "first value: " << sim.simulatedValues.front()
              << " (expected around " << startPrice << ")\n";

    // 4. Print the min and max reached, just to see the range.
    auto [mn, mx] = std::minmax_element(
        sim.simulatedValues.begin(), sim.simulatedValues.end());
    std::cout << "min: " << *mn << "  max: " << *mx << "\n";

    // 5. Peek at the first and last few values.
    std::cout << "head: ";
    for (int i = 0; i < 5; ++i) std::cout << sim.simulatedValues[i] << " ";
    std::cout << "\nlast: " << sim.simulatedValues.back() << "\n";

    // --- Write a CSV so the path can be plotted later ---
    std::ofstream out("gbm_output.csv");
    out << "time,value\n";
    for (std::size_t i = 0; i < sim.timePoints.size(); ++i) {
        out << sim.timePoints[i] << "," << sim.simulatedValues[i] << "\n";
    }
    std::cout << "CSV written to gbm_output.csv\n";

    return 0;
}