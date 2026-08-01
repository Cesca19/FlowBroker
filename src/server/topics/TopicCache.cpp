//
// Created by fran on 27/07/2026.
//

#include "TopicCache.hpp"

TopicCache::TopicCache()
{
}

void TopicCache::addTopicSample(const std::string &topicName, const double value, const std::uint64_t timestampNs) {
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end())
        it = m_topicStatesCache.emplace(topicName, TopicState(topicName)).first;
    it->second.addSample(value, timestampNs);
}

TopicSnapshot TopicCache::getTopicSnapshot(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    const auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end())
        return TopicSnapshot();
    const TopicState &topicState = it->second;
    return TopicSnapshot { topicState.name(), topicState.lastTimestampNs(), topicState.lastValue(),
        topicState.average(), topicState.min(), topicState.max() };
}

std::vector<TopicSnapshot> TopicCache::getAllTopicsSnapshot() const
{
    std::vector<TopicSnapshot> topicSnapshots;
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);

    if (m_topicStatesCache.empty())
        return topicSnapshots;
    for (const auto &[name, topicState] : m_topicStatesCache)
        topicSnapshots.push_back({ topicState.name(), topicState.lastTimestampNs(), topicState.lastValue(),
            topicState.average(), topicState.min(), topicState.max() });
    return topicSnapshots;
}
