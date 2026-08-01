//
// Created by fran on 26/07/2026.
//

#include "TopicState.hpp"

TopicState::TopicState(const std::string &topicName)
    : m_topicName(topicName)
    , m_recentSamplesDurationInSec(5)
{
}

void TopicState::addSample(const double value, const std::uint64_t timestampNs)
{
    m_recentSamples.push_back({value, timestampNs});
    const std::uint64_t maxDurationNs = static_cast<std::uint64_t>(m_recentSamplesDurationInSec) * 1'000'000'000ULL;

    while (!m_recentSamples.empty() && m_recentSamples.front().timestampNs + maxDurationNs < timestampNs)
        m_recentSamples.pop_front();
}

double TopicState::min() const
{
    if (m_recentSamples.empty())
        return 0.0;
    double min = m_recentSamples.front().value;
    for (const auto &[value, timestampNs] : m_recentSamples)
        if (value < min)
            min = value;
    return min;
}

double TopicState::max() const
{
    if (m_recentSamples.empty())
        return 0.0;
    double max = m_recentSamples.front().value;
    for (const auto &[value, timestampNs] : m_recentSamples)
        if (value > max)
            max = value;
    return max;
}

double TopicState::average() const
{
    if (m_recentSamples.empty())
        return 0.0;
    double sum = 0.0;
    for (const auto&[value, timestampNs] : m_recentSamples)
        sum += value;
    return sum / m_recentSamples.size();
}

double TopicState::lastValue() const
{
    if (m_recentSamples.empty())
        return 0.0;
    return m_recentSamples.back().value;
}

std::string TopicState::name() const
{
    return m_topicName;
}

std::uint64_t TopicState::lastTimestampNs() const
{
    if (m_recentSamples.empty())
        return 0;
    return m_recentSamples.back().timestampNs;
}
