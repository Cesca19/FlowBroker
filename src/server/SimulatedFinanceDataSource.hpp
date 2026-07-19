//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP
#define FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP

#include "SimulatedDataSource.hpp"
#include "SimulatedFinancialAsset.hpp"

class SimulatedFinanceDataSource : public SimulatedDataSource {
public:
    SimulatedFinanceDataSource(boost::asio::io_context& ioContext, boost::asio::chrono::milliseconds periodInMilliseconds,
        const FinancialAssetConfig& config);
private:
    Message generateMessage() override;

    SimulatedFinancialAsset m_simulatedFinancialAsset;
};


#endif //FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP
