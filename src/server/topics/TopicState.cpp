//
// Created by fran on 26/07/2026.
//

#include "TopicState.hpp"
#include <iostream>

TopicState::TopicState(const TopicDescriptor &descriptor, const unsigned int topicId)
    : m_Id(topicId)
    , m_topicDescriptor(descriptor)
    , m_recentSamplesDurationInSec(5)
{
}

void TopicState::addSample(const std::vector<double> &values, const std::uint64_t timestampNs)
{
    if (m_topicDescriptor.schema.size() != values.size()) {
        std::cerr << "TopicState: Invalid number of values in topic " + m_topicDescriptor.name 
        + " expected " + std::to_string(m_topicDescriptor.schema.size()) + " value(s)" 
        << std::endl;
        return;
    }
    m_recentReadings.push_back({values, timestampNs});
    const std::uint64_t maxDurationNs = m_recentSamplesDurationInSec * 1'000'000'000ULL;
    while (!m_recentReadings.empty() && m_recentReadings.front().timestampNs + maxDurationNs < timestampNs)
            m_recentReadings.pop_front();
}

std::vector<double> TopicState::min() const
{
    std::vector<double> minValues;

    if (m_recentReadings.empty())
        return minValues;
    minValues = m_recentReadings.front().values;
    for (const auto & [values, timestampNs] : m_recentReadings)
        for (std::size_t i = 0; i < values.size(); i++)
            if (values[i] < minValues[i])
                minValues[i] = values[i];
    return minValues;
}

std::vector<double> TopicState::max() const
{
    std::vector<double> maxValues;

    if (m_recentReadings.empty())
        return maxValues;
    maxValues = m_recentReadings.front().values;
    for (const auto & [values, timestampNs] : m_recentReadings)
        for (std::size_t i = 0; i < values.size(); i++)
            if (values[i] > maxValues[i])
                maxValues[i] = values[i];
    return maxValues;
}

std::vector<double> TopicState::average() const
{
    if (m_recentReadings.empty())
        return {};
    
    std::vector<double> averageValues(m_topicDescriptor.schema.size(), 0);

    for (const auto & [values, timestampNs] : m_recentReadings)
        for (std::size_t i = 0; i < values.size(); i++)
            averageValues[i] += values[i];

    for (auto &value : averageValues)
        value = value / m_recentReadings.size();
    return averageValues;
}

std::vector<double> TopicState::lastValue() const
{
    return m_recentReadings.empty() ? std::vector<double>() : m_recentReadings.back().values;
}

StreamType TopicState::type() const
{
    return m_topicDescriptor.type;
}

std::string TopicState::name() const
{
    return m_topicDescriptor.name;
}

unsigned int TopicState::id() const
{
    return m_Id;
}

std::uint64_t TopicState::lastTimestampNs() const
{
    if (m_recentReadings.empty())
        return 0;
    return m_recentReadings.back().timestampNs;
}

TopicDescriptor TopicState::descriptor() const
{
    return m_topicDescriptor;
}
