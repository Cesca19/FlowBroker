//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_FINANCIALASSETCONFIG_HPP
#define FLOWBROKER_FINANCIALASSETCONFIG_HPP

#include <string>

/**
 * @struct FinancialAssetConfig
 * @brief The parameters that define one financial simulated asset.
 *
 * It holds the data passed to SimulatedFinancialAsset, which uses these values to
 * generate a price series with geometric brownian motion. drift and volatility
 * are expressed per second, so they are independent of the tick rate.
 */
struct FinancialAssetConfig {
    std::string name;       ///< Topic name for this asset, e.g. "AAPL".
    double initialPrice;    ///< Starting price of the series.
    double drift;           ///< Expected growth rate per second
    double volatility;      ///< Standard deviation of the growth rate per second
};

#endif //FLOWBROKER_FINANCIALASSETCONFIG_HPP
