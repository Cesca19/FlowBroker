//
// Created by fran on 16/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDFINANCIALASSET_HPP
#define FLOWBROKER_SIMULATEDFINANCIALASSET_HPP

#include <random>
#include "FinancialAssetConfig.hpp"

/**
 * @class SimulatedFinancialAsset
 * @brief Generates a realistic price series for one asset, tick by tick.
 *
 * Holds the current price and produces the next one on demand using geometric
 * brownian motion.
 * This class is pure price maths, with no notion of topics, timers or the
 * pipeline; SimulatedFinanceDataSource wraps it into a data source.
 */
class SimulatedFinancialAsset {
public:
    /**
     * @param config             Asset parameters (start price, drift, volatility).
     * @param deltaTimeInSeconds Time one tick represents, in seconds. Used to
     *                           scale drift and volatility to the tick rate.
     */
    SimulatedFinancialAsset(const FinancialAssetConfig& config, double deltaTimeInSeconds);
    
    /// Advance the simulation by one step and return the new price.
    double getNextPrice();
    
    /// The current price, without advancing the simulation.
    double getCurrentPrice() const;
    
    /// The asset's name (used as the topic name).
    std::string getName() const;
private:
     /// Set up the Gaussian generator used for the random shocks.
    void initGaussianFunctionGenerator(double mean = 0.0, double standardDeviation = 1.0);
    
    /// Draw one random value from the normal distribution.
    double getGaussianRandomValue();

    std::string m_name;
    double m_currentPrice;
    double m_drift;
    double m_volatility;
    double m_deltaTime;
    std::mt19937 m_randomEngine;
    std::normal_distribution<double> m_normalDistribution;
};


#endif //FLOWBROKER_SIMULATEDFINANCIALASSET_HPP