//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_SIMULATEDDATASOURCE_HPP
#define FLOWBROKER_SIMULATEDDATASOURCE_HPP

#include "IDataSource.hpp"
#include <boost/asio.hpp>

class SimulatedDataSource : public IDataSource {
public:
    SimulatedDataSource(boost::asio::io_context& ioContext, boost::asio::chrono::milliseconds periodInMilliseconds);
    void start(std::function<void(const Message &)> deliverMessage) override;
    void stop() override;
private:
    void sendNewMessage(const boost::system::error_code &error);
    virtual Message generateMessage() = 0;
protected:
    static std::uint64_t getCurrentTimeAsNanoseconds();

    bool m_isRunning;
    boost::asio::io_context& m_ioContext;
    boost::asio::steady_timer m_timer;
    boost::asio::chrono::milliseconds m_periodInMilliseconds;
    std::function<void(const Message &)> m_deliverMessage;
};


#endif //FLOWBROKER_SIMULATEDDATASOURCE_HPP
