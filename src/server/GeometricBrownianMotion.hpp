//
// Created by fran on 16/07/2026.
//

#ifndef FLOWBROKER_GEOMETRICBROWNIANMOTION_HPP
#define FLOWBROKER_GEOMETRICBROWNIANMOTION_HPP

#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

struct FinancialAssetConfig {
    std::string name;
    double initialPrice;
    double drift;
    double volatility;
};

namespace GBM {
    struct GBMSimulation {
        std::vector<double> timePoints; // x axis
        std::vector<double> simulatedValues; // y axis
    };

    std::vector<double> expOfVector(const std::vector<double>& inputVector);
    std::vector<double> cumulativeSum(const std::vector<double>& inputVector);
    std::vector<double> generateTimePoints(int nbSteps, double timeInterval);
    std::vector<double> multiplyVectorByScalar(const std::vector<double>& inputVector, double scalar);
    std::function<double()> gaussianFunctionGenerator(double mean = 0.0, double standardDeviation = 1.0);
    GBMSimulation simulateGeometricBrownianMotion(int nbSteps, double timeInterval, double drift, 
        double volatility, double startPrice, std::function<double()> gaussianFunctionGenerator);
    void testGMBSimulation();
};


#endif //FLOWBROKER_GEOMETRICBROWNIANMOTION_HPP