//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_IDATASOURCE_HPP
#define FLOWBROKER_IDATASOURCE_HPP

#include <functional>
#include "../../common/Message.hpp"
#include "../../common/TopicDescriptor.hpp"

/**
 * @interface IDataSource
 * @brief The contract every data source obeys, whatever its origin.
 *
 * A data source produces the values of one or more topics: a GBM simulation, a
 * live finance WebSocket, a weather HTTP feed. The server depends only on this
 * interface and holds its sources as a list of IDataSource, so a new kind of
 * source can be added without touching the pipeline. This is what makes the
 * broker domain-agnostic.
 *
 * A source hands each value to the pipeline as a Message through the callback
 * given to start(); it never talks to the network or the cache directly.
 */
class IDataSource {
public:
    virtual ~IDataSource() = default;

    /**
     * @brief Start producing values. Each value produced is passed to the
     *        given callback, one call per value.
     * @param deliverMessage The function to call with every message produced.
     */
    virtual void start(std::function<void(const Message&)> deliverMessage) = 0;

     /**
     * @brief Stop producing. No more values are delivered after this.
     */
    virtual void stop() = 0;

    /**
     * @brief The topics this source will feed and their types.
     * @return One entry per topic the source provides.
     */
    virtual std::vector<TopicDescriptor> providedTopics() const = 0;
};

#endif //FLOWBROKER_IDATASOURCE_HPP
