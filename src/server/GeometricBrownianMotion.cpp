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

    void testGMBSimulation()
    {
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
    }
};