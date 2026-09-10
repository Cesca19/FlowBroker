//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_TOPICSTATE_HPP
#define FLOWBROKER_TOPICSTATE_HPP

#include <cstdint>
#include <deque>
#include "../../common/Message.hpp"
#include "../../common/TopicDescriptor.hpp"

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
     * @param descriptor Everything known about a topic at declaration time
     * @param type       Its stream type (finance, weather, sensor).
     */
    TopicState(const TopicDescriptor &descriptor, const unsigned int topicId);

    /// Add a sample, then drop any that fell outside the time window.
    void addSample(const std::vector<double> &values, std::uint64_t timestampNs);
    
    std::vector<double> min() const;         ///< Smallest values in the window ({0} if empty).
    std::vector<double> max() const;         ///< Largest values in the window ({0} if empty).
    std::vector<double> average() const;     ///< Mean of the values in the window ({0} if empty).
    std::vector<double> lastValue() const;   ///< Most recent values ({0} if empty).
    StreamType type() const;    ///< The topic's stream type.
    std::string name() const;   ///< The topic's name.
    unsigned int id() const;    ///< The topic's ID.
    std::uint64_t lastTimestampNs() const;  ///< Timestamp of the most recent sample (0 if empty).
    TopicDescriptor descriptor() const;
private:
    const unsigned int m_Id;
    TopicDescriptor m_topicDescriptor;
    int m_recentSamplesDurationInSec;       ///< Width of the sliding window, in seconds.
    std::vector<std::deque<Sample>> m_recentSamplesByField;  ///< Samples within the window, oldest first, one deque per field.
    // std::deque<Sample> m_recentSamples;     ///< Samples within the window, oldest first.
};


#endif //FLOWBROKER_TOPICSTATE_HPP
