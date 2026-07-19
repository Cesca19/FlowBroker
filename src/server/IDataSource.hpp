//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_IDATASOURCE_HPP
#define FLOWBROKER_IDATASOURCE_HPP

#include <functional>
#include "Message.hpp"

// The contract every data source obeys, whether it wraps a GBM simulation or a
// live WebSocket feed. The server depends only on this interface: it holds a list of IDataSource

class IDataSource {
public:
    virtual ~IDataSource() = default;
    virtual void start(std::function<void(const Message&)> deliverMessage) = 0;
    virtual void stop() = 0;
};

#endif //FLOWBROKER_IDATASOURCE_HPP
