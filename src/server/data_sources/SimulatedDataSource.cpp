//
// Created by fran on 19/07/2026.
//

#include "SimulatedDataSource.hpp"

SimulatedDataSource::SimulatedDataSource(boost::asio::io_context &ioContext, const boost::asio::chrono::milliseconds periodInMilliseconds)
    : m_isRunning(false)
    , m_ioContext(ioContext)
    , m_timer(m_ioContext)
    , m_periodInMilliseconds(periodInMilliseconds)
    , m_deliverMessage(nullptr)
{
}

void SimulatedDataSource::start(std::function<void(const Message &)> deliverMessage)
{
    m_deliverMessage = std::move(deliverMessage);
    m_isRunning = true;
    m_timer.expires_after(m_periodInMilliseconds);
    m_timer.async_wait(std::bind(&SimulatedDataSource::sendNewMessage, this, boost::asio::placeholders::error));
}

void SimulatedDataSource::stop()
{
    m_timer.cancel();
    m_deliverMessage = nullptr;
    m_isRunning = false;
}

void SimulatedDataSource::sendNewMessage(const boost::system::error_code &error)
{
    if (error || !m_deliverMessage || !m_isRunning)
        return;

    m_deliverMessage(generateMessage());
    m_timer.expires_at(m_timer.expiry() + m_periodInMilliseconds);
    m_timer.async_wait(std::bind(&SimulatedDataSource::sendNewMessage, this, boost::asio::placeholders::error));
}

std::uint64_t SimulatedDataSource::getCurrentTimeAsNanoseconds()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
}
