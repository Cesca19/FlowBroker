//
// Created by fran on 19/07/2026.
//

#include "SimulatedFinanceDataSource.hpp"

SimulatedFinanceDataSource::SimulatedFinanceDataSource(boost::asio::io_context &ioContext,
    const boost::asio::chrono::milliseconds periodInMilliseconds, const FinancialAssetConfig &config)
    : SimulatedDataSource(ioContext, periodInMilliseconds)
    , m_simulatedFinancialAsset(config)
{
}

Message SimulatedFinanceDataSource::generateMessage()
{
    const double nextPrice = m_simulatedFinancialAsset.getNextPrice();
    Message message {
        .type = StreamType::FINANCE,
        .topic = m_simulatedFinancialAsset.getName(),
        .values = std::vector<double>{nextPrice},
        .timestampNs = getCurrentTimeAsNanoseconds(),
    };
    return message;
}
