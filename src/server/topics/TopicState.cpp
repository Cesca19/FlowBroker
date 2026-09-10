//
// Created by fran on 26/07/2026.
//

#include "TopicState.hpp"
#include <iostream>

TopicState::TopicState(const TopicDescriptor &descriptor, const unsigned int topicId)
    : m_Id(topicId)
    , m_topicDescriptor(descriptor)
    , m_recentSamplesDurationInSec(5)
    , m_recentSamplesByField(descriptor.schema.size())
{
}

void TopicState::addSample(const std::vector<double> &values, const std::uint64_t timestampNs)
{
    if (m_recentSamplesByField.size() != values.size()) {
        std::cerr << "TopicState: Invalid number of values in topic " + m_topicDescriptor.name 
        + " expected " + std::to_string(m_topicDescriptor.schema.size()) + " value(s)" 
        << std::endl;
        return;
    }

    for (std::size_t i = 0; i < m_recentSamplesByField.size() ; i++) {
        auto &currentFieldSamples = m_recentSamplesByField[i];
        const std::uint64_t maxDurationNs = static_cast<std::uint64_t>(m_recentSamplesDurationInSec) * 1'000'000'000ULL;

        currentFieldSamples.push_back({values[i], timestampNs});
        while (!currentFieldSamples.empty() && currentFieldSamples.front().timestampNs + maxDurationNs < timestampNs)
            currentFieldSamples.pop_front();
    }
}

std::vector<double> TopicState::min() const
{
    std::vector<double> minValues;
    for (std::size_t i = 0; i < m_recentSamplesByField.size() ; i++) {
        auto &currentFieldSamples = m_recentSamplesByField[i];

        if (currentFieldSamples.empty()) {
            minValues.push_back(0);
            continue;
        }
        double min = currentFieldSamples.front().value;
        for (const auto &[value, timestampNs] : currentFieldSamples)
            if (value < min)
                min = value;
        minValues.push_back(min);
    }
    return minValues;
}

std::vector<double> TopicState::max() const
{
    std::vector<double> maxValues;
    for (std::size_t i = 0; i < m_recentSamplesByField.size() ; i++) {
        auto &currentFieldSamples = m_recentSamplesByField[i];
        if (currentFieldSamples.empty()) {
            maxValues.push_back(0);
            continue;
        }
        double max = currentFieldSamples.front().value;
        for (const auto &[value, timestampNs] : currentFieldSamples)
            if (value > max)
                max = value;
        maxValues.push_back(max);
    }
    return maxValues;
}

std::vector<double> TopicState::average() const
{
    std::vector<double> averageValues;
    for (std::size_t i = 0; i < m_recentSamplesByField.size() ; i++) {
        auto &currentFieldSamples = m_recentSamplesByField[i];
        if (currentFieldSamples.empty()) {
            averageValues.push_back(0);
            continue;
        }
        double sum = 0.0;
        for (const auto &[value, timestampNs] : currentFieldSamples)
            sum += value;
        averageValues.push_back(sum / currentFieldSamples.size());
    }
    return averageValues;
}

std::vector<double> TopicState::lastValue() const
{
    std::vector<double> lastValues;
    for (std::size_t i = 0; i < m_recentSamplesByField.size() ; i++) {
        auto &currentFieldSamples = m_recentSamplesByField[i];
        if (currentFieldSamples.empty()) {
            lastValues.push_back(0);
            continue;
        }
        lastValues.push_back(currentFieldSamples.back().value);
    }
    return lastValues;
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
    if (m_recentSamplesByField.empty())
        return 0;
    return m_recentSamplesByField[0].empty() ? 0 : m_recentSamplesByField[0].back().timestampNs;
}

TopicDescriptor TopicState::descriptor() const
{
    return m_topicDescriptor;
}
