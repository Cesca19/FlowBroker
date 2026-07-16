//
// Created by fran on 16/07/2026.
//

#include "GeometricBrownianMotion.hpp"

namespace GBM {
    std::vector<double> expOfVector(const std::vector<double> &inputVector)
    {
        std::vector<double> result;
        for (const auto &value : inputVector) {
            result.push_back(std::exp(value));
        }
        return result;
    }

    std::vector<double> cumulativeSum(const std::vector<double> &inputVector)
    {
        std::vector<double> result;
        double sum = 0;
        for (const auto &value : inputVector) {
            sum += value;
            result.push_back(sum);
        }
        return result;
    }

    std::vector<double> generateTimePoints(int nbSteps, double timeInterval)
    {
        std::vector<double> timePoints(nbSteps);
        for (int i = 0; i < nbSteps; i++)
            timePoints[i] = i * timeInterval;
        return timePoints;
    }

    std::vector<double> multiplyVectorByScalar(const std::vector<double> &inputVector, double scalar)
    {
        std::vector<double> result(inputVector.size());
        for (std::size_t i = 0; i < inputVector.size(); i++)
            result[i] = inputVector[i] * scalar;
        return result;
    }

    std::function<double()> gaussianFunctionGenerator(double mean, double standardDeviation)
    {
        // It is a uniformly-distributed integer random number generator that produces non-deterministic random numbers
        std::random_device randomDevice{};
        // It is a random number engine based on Mersenne Twister algorithm. It produces unsigned integer random numbers 
        // randomDevice() is used to seed the random engine, ensuring that the generated numbers are different each time the program is run
        std::mt19937 randomEngine{randomDevice()};
        // Normal distribution generates random numbers according to the Normal (or Gaussian) random number distribution
        std::normal_distribution<double> normalDistribution{mean, standardDeviation};

        // 'mutable' is required here: drawing a value advances the engine's internal
        // state (Mersenne Twister remembers its position so the next draw differs).
        // By default a lambda's captured-by-value copies are read-only, so mutating
        // them would not compile. 'mutable' lets the lambda modify its OWN copies of
        // randomEngine and normalDistribution, so each call yields a new number.
        std::function<double()> gaussianFunction = [normalDistribution, randomEngine]() mutable {
            // Generate a random number from the normal distribution
            return normalDistribution(randomEngine);
        };
        return gaussianFunction;
    }

    GBMSimulation simulateGeometricBrownianMotion(int nbSteps, double timeInterval, double drift, 
        double volatility, double startPrice, std::function<double()> gaussianFunctionGenerator)
    {
        GBMSimulation simulation;
        std::vector<double> steps(nbSteps);
        for (int i = 0; i < nbSteps; i++) {
            double deterministicPart = drift - (std::pow(volatility, 2) / 2);
            double stochasticPart = gaussianFunctionGenerator() * volatility;
            double step = deterministicPart  + stochasticPart;
            steps[i] = step;
        }
        std::vector<double> cumulativeSteps = cumulativeSum(steps);
        std::vector<double> simulatedValues = multiplyVectorByScalar(expOfVector(cumulativeSteps), startPrice);
        std::vector<double> timePoints = generateTimePoints(nbSteps, timeInterval);
        
        simulation.timePoints = timePoints;
        simulation.simulatedValues = simulatedValues;
        return simulation;
    }
};