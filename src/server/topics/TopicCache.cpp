//
// Created by fran on 27/07/2026.
//

#include <iostream>
#include "TopicCache.hpp"

TopicCache::TopicCache()
    : m_nextTopicId(1)
{
}

void TopicCache::addTopic(const TopicDescriptor &topicDescriptor)
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    if (m_topicStatesCache.find(topicDescriptor.name) == m_topicStatesCache.end())
        m_topicStatesCache.emplace(topicDescriptor.name, TopicState(topicDescriptor, m_nextTopicId++));
}

void TopicCache::addTopicSample(const std::string &topicName, const std::vector<double> &values, const std::uint64_t timestampNs) {
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end()) {
        std::cerr << "TopicCache: undeclared topic " << topicName << std::endl;
        return;
    }
    it->second.addSample(values, timestampNs);
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
        topics.push_back(topicState.descriptor());
    return topics;
}

bool TopicCache::hasTopic(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    
    return m_topicStatesCache.find(topicName) != m_topicStatesCache.end();
}

unsigned int TopicCache::topicId(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);
    
    if (it == m_topicStatesCache.end())
        return 0;
    return it->second.id();
}

std::vector<std::string> TopicCache::topicSchema(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end())
        return std::vector<std::string>();
    return it->second.descriptor().schema;
}

StreamType TopicCache::topicType(const std::string &topicName) const
{
    std::lock_guard<std::mutex> lockGuard(m_topicStateCacheMutex);
    auto it = m_topicStatesCache.find(topicName);

    if (it == m_topicStatesCache.end())
        return StreamType::UNKNOWN;
    return it->second.descriptor().type;
}
