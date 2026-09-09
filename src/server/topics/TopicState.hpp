//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_TOPICSTATE_HPP
#define FLOWBROKER_TOPICSTATE_HPP

#include <cstdint>
#include <deque>
#include <string>
#include "../../common/Message.hpp"

/// One data point of a topic: a value and when it was produced.
struct Sample {
    double value;
    std::uint64_t timestampNs;
};

/**
 * @class TopicState
 * @brief The recent history and live stats of a single topic.
 *
 * Keeps a sliding window of the last few seconds of samples and 
 * computes simple stats over it (min, max, average, last value).
 * Old samples are dropped as new ones arrive, so the window always 
 * covers roughly the same time span, not a fixed number of points.
 *
 * It is owned by TopicCache, which is responsible for thread-safety
 * and lifetime management.
 */
class TopicState {
public:
    /**
     * @param topicName  The topic's name.
     * @param type       Its stream type (finance, weather, sensor).
     */
    TopicState(const std::string &topicName, StreamType type);

    /// Add a sample, then drop any that fell outside the time window.
    void addSample(double value, std::uint64_t timestampNs);
    
    double min() const;         ///< Smallest value in the window (0 if empty).
    double max() const;         ///< Largest value in the window (0 if empty).
    double average() const;     ///< Mean of the values in the window (0 if empty).
    double lastValue() const;   ///< Most recent value (0 if empty).
    StreamType type() const;    ///< The topic's stream type.
    std::string name() const;   ///< The topic's name.
    std::uint64_t lastTimestampNs() const;  ///< Timestamp of the most recent sample (0 if empty).
private:
    std::string m_topicName;
    StreamType m_type;
    int m_recentSamplesDurationInSec;       ///< Width of the sliding window, in seconds.
    std::deque<Sample> m_recentSamples;     ///< Samples within the window, oldest first.
};


#endif //FLOWBROKER_TOPICSTATE_HPP
