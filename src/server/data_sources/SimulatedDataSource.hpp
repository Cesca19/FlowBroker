//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDDATASOURCE_HPP
#define FLOWBROKER_SIMULATEDDATASOURCE_HPP

#include "IDataSource.hpp"
#include <boost/asio.hpp>


/**
 * @class SimulatedDataSource
 * @brief Base for every simulated source: ticks on a timer and delivers a value
 *        each time.
 *
 * This class does the work shared by all simulations: it runs a repeating timer
 * and, on every tick, produces one value and passes it to the callback. What it
 * does NOT know is what value to produce; each concrete source fills that in by
 * overriding generateMessage(). (SimulatedFinanceDataSource, for example,
 * returns a simulated price.)
 *
 * The timer runs on the shared io_context, so ticks happen on whatever thread
 * drives it (the server thread here), not on a thread of its own.
 */
class SimulatedDataSource : public IDataSource {
public:
    /**
     * @param ioContext             The io_context whose thread drives the timer.
     * @param periodInMilliseconds  Delay between two ticks (the tick rate).
     */
    SimulatedDataSource(boost::asio::io_context& ioContext, boost::asio::chrono::milliseconds periodInMilliseconds);
    
    /// Start ticking. Stores the callback and arms the first timer.
    void start(std::function<void(const Message &)> deliverMessage) override;
    
    /// Stop ticking. No more values are delivered after this.
    void stop() override;
private:
    /// Called on every timer tick: delivers one value, then re-arms the timer.
    void sendNewMessage(const boost::system::error_code &error);

     /**
     * @brief Produce the next value. Each concrete source implements this.
     * @return The Message to deliver for this tick.
     */
    virtual Message generateMessage() = 0;
protected:
    /// Current time since epoch, in nanoseconds. For children stamping messages.
    static std::uint64_t getCurrentTimeAsNanoseconds();

    bool m_isRunning;
    boost::asio::io_context& m_ioContext;
    boost::asio::steady_timer m_timer;
    boost::asio::chrono::milliseconds m_periodInMilliseconds;
    std::function<void(const Message &)> m_deliverMessage;
};


#endif //FLOWBROKER_SIMULATEDDATASOURCE_HPP
