//
// Created by fran on 01/08/2026.
//

#ifndef FLOWBROKER_TOPICSNAPSHOT_HPP
#define FLOWBROKER_TOPICSNAPSHOT_HPP

#include <cstdint>
#include <string>

struct TopicSnapshot {
    std::string topicName;
    std::uint64_t timestampNs;
    double lastValue;
    double average;
    double min;
    double max;
};

#endif //FLOWBROKER_TOPICSNAPSHOT_HPP
