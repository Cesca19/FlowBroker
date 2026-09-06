//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_TOPICSTATE_HPP
#define FLOWBROKER_TOPICSTATE_HPP

#include <cstdint>
#include <deque>
#include <string>
#include "../../common/Message.hpp"

struct Sample {
    double value;
    std::uint64_t timestampNs;
};

class TopicState {
public:
    TopicState(const std::string &topicName, StreamType type);
    void addSample(double value, std::uint64_t timestampNs);
    double min() const;
    double max() const;
    double average() const;
    double lastValue() const;
    StreamType type() const;
    std::string name() const;
    std::uint64_t lastTimestampNs() const;
private:
    std::string m_topicName;
    StreamType m_type;
    int m_recentSamplesDurationInSec;
    std::deque<Sample> m_recentSamples;
};


#endif //FLOWBROKER_TOPICSTATE_HPP
