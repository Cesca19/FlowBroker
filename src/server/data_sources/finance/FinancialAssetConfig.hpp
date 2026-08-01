//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_FINANCIALASSETCONFIG_HPP
#define FLOWBROKER_FINANCIALASSETCONFIG_HPP

#include <string>

struct FinancialAssetConfig {
    std::string name;
    double initialPrice;
    double drift;
    double volatility;
};

#endif //FLOWBROKER_FINANCIALASSETCONFIG_HPP
