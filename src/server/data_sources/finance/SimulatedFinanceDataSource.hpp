//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP
#define FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP

#include "../SimulatedDataSource.hpp"
#include "SimulatedFinancialAsset.hpp"

/**
 * @class SimulatedFinanceDataSource
 * @brief A simulated source that produces the price of one financial asset.
 *
 * On each tick it asks its SimulatedFinancialAsset for the next price 
 * and wraps it in a Message. 
 */
class SimulatedFinanceDataSource : public SimulatedDataSource {
public:
    /**
     * @param ioContext             Context whose thread drives the timer.
     * @param periodInMilliseconds  Delay between two price updates.
     * @param config                The asset to simulate (name, start price,
     *                              drift, volatility).
     */
    SimulatedFinanceDataSource(boost::asio::io_context& ioContext, boost::asio::chrono::milliseconds periodInMilliseconds,
        const FinancialAssetConfig& config);

    /// One topic: the asset's name, of type FINANCE.
    std::vector<TopicDescriptor> providedTopics() const override;
private:
    /// Produce the next price as a Message (one value: the price).
    Message generateMessage() override;
    
    SimulatedFinancialAsset m_simulatedFinancialAsset;
};


#endif //FLOWBROKER_SIMULATEDFINANCEDATASOURCE_HPP
