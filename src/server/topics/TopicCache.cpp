//
// Created by fran on 27/07/2026.
//

#include "TopicCache.hpp"

TopicCache::TopicCache()
{
}

void TopicCache::addTopic(const std::string &topicName, const StreamType type)
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    if (m_topicStatesCache.find(topicName) == m_topicStatesCache.end())
        m_topicStatesCache.emplace(topicName, TopicState(topicName, type));
}

void TopicCache::addTopicSample(const std::string &topicName, const StreamType type, const double value, const std::uint64_t timestampNs) {
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end())
        it = m_topicStatesCache.emplace(topicName, TopicState(topicName, type)).first;
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

std::vector<TopicDescriptor> TopicCache::topics() const
{
    std::vector<TopicDescriptor> topics;
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    
    for (const auto &[name, topicState] : m_topicStatesCache)
        topics.push_back(TopicDescriptor({topicState.name(), topicState.type()}));
    return topics;
}

bool TopicCache::hasTopic(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    
    return m_topicStatesCache.find(topicName) != m_topicStatesCache.end();
}
