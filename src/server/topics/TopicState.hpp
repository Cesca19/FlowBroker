//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_TOPICSTATE_HPP
#define FLOWBROKER_TOPICSTATE_HPP

#include <cstdint>
#include <deque>
#include <string>

struct Sample {
    double value;
    std::uint64_t timestampNs;
};

class TopicState {
public:
    TopicState(const std::string &topicName);
    void addSample(double value, std::uint64_t timestampNs);
    double min() const;
    double max() const;
    double average() const;
    double lastValue() const;
    std::string name() const;
private:
    std::string m_topicName;
    int m_recentSamplesDurationInSec;
    std::deque<Sample> m_recentSamples;
};


#endif //FLOWBROKER_TOPICSTATE_HPP
