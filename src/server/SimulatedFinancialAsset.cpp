//
// Created by fran on 16/07/2026.
//

#include "SimulatedFinancialAsset.hpp"

SimulatedFinancialAsset::SimulatedFinancialAsset(const FinancialAssetConfig &config)
    : m_name(config.name),
      m_initialPrice(config.initialPrice),
      m_currentPrice(config.initialPrice),
      m_drift(config.drift),
      m_volatility(config.volatility)
{
    initGaussianFunctionGenerator();
}

double SimulatedFinancialAsset::getNextPrice()
{
    double deterministicPart = m_drift - (std::pow(m_volatility, 2) / 2);
    double stochasticPart = getGaussianRandomValue() * m_volatility;
    double step = deterministicPart  + stochasticPart;
    double nextPrice = m_currentPrice * std::exp(step);
    m_currentPrice = nextPrice;
    return nextPrice;
}

double SimulatedFinancialAsset::getCurrentPrice() const
{
    return m_currentPrice;
}

std::string SimulatedFinancialAsset::getName() const
{
    return m_name;
}

void SimulatedFinancialAsset::initGaussianFunctionGenerator(const double mean, const double standardDeviation)
{
    // It is a uniformly-distributed integer random number generator that produces non-deterministic random numbers
    std::random_device randomDevice{};
    // It is a random number engine based on Mersenne Twister algorithm. It produces unsigned integer random numbers 
    // randomDevice() is used to seed the random engine, ensuring that the generated numbers are different each time the program is run
    m_randomEngine = std::mt19937(randomDevice());
    // Normal distribution generates random numbers according to the Normal (or Gaussian) random number distribution
    m_normalDistribution = std::normal_distribution<double>(mean, standardDeviation);
}

double SimulatedFinancialAsset::getGaussianRandomValue()
{
    // Generate a random number from the normal distribution
    return m_normalDistribution(m_randomEngine);
}
