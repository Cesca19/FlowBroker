//
// Created by fran on 16/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDFINANCIALASSET_HPP
#define FLOWBROKER_SIMULATEDFINANCIALASSET_HPP

#include "GeometricBrownianMotion.hpp"

class SimulatedFinancialAsset {
public:
    SimulatedFinancialAsset(const FinancialAssetConfig& config);
    double getNextPrice();
    double getCurrentPrice() const;
private:
    void initGaussianFunctionGenerator(double mean = 0.0, double standardDeviation = 1.0);
    double getGaussianRandomValue();

    std::string m_name;
    double m_initialPrice;
    double m_currentPrice;
    double m_drift;
    double m_volatility;
    std::mt19937 m_randomEngine;
    std::normal_distribution<double> m_normalDistribution;
};


#endif //FLOWBROKER_SIMULATEDFINANCIALASSET_HPP