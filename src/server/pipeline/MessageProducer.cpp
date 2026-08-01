//
// Created by fran on 25/07/2026.
//

#include "MessageProducer.hpp"
#include "../data_sources/finance/SimulatedFinanceDataSource.hpp"

MessageProducer::MessageProducer(boost::asio::io_context &ioContext, MessageCatalog &messageCatalog)
    : m_ioContext(ioContext)
    , m_messageCatalog(messageCatalog)
{
}

void MessageProducer::start()
{
    initDataSources();
    startDataSources();
}

void MessageProducer::stop() const
{
    stopDataSources();
}

void MessageProducer::addMessage(const Message &message) const
{
    m_messageCatalog.push(message);
}

void MessageProducer::initDataSources()
{
    FinancialAssetConfig goldConfig{"GOLD", 180.0, 0.0001, 0.02};
    FinancialAssetConfig sp500Config{"S&P", 5800.0, 0.00002, 0.004};
    FinancialAssetConfig bitCoinConfig{"BTC", 95000.0, 0.00010, 0.030};
    FinancialAssetConfig appleConfig{"AAPL", 225.0, 0.00005, 0.010};
    FinancialAssetConfig eurUsdConfig{"EURUSD", 1.08, 0.00001, 0.002};
    FinancialAssetConfig crudeOilConfig{"OIL", 78.0, 0.00003, 0.015};

    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(20), goldConfig));
    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(15), sp500Config));
    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(5), bitCoinConfig));
    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(10), appleConfig));
    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(50), eurUsdConfig));
    m_dataSources.push_back(std::make_unique<SimulatedFinanceDataSource>(m_ioContext, boost::asio::chrono::milliseconds(25), crudeOilConfig));
}

void MessageProducer::startDataSources() const
{
    for (const auto& dataSource : m_dataSources)
        dataSource->start([this](const Message& message) {
            addMessage(message);
        });
}

void MessageProducer::stopDataSources() const
{
    for (const auto& dataSource : m_dataSources)
        dataSource->stop();
}
