//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TOPICCACHE_HPP
#define FLOWBROKER_TOPICCACHE_HPP

#include <mutex>
#include <vector>
#include <unordered_map>
#include "TopicState.hpp"
#include "../../common/TopicSnapshot.hpp"

class TopicCache {
public:
    TopicCache();
    void addTopicSample(const std::string &topicName, double value, std::uint64_t timestampNs);
    TopicSnapshot getTopicSnapshot(const std::string &topicName) const;
    std::vector<TopicSnapshot> getAllTopicsSnapshot() const;
private:
    std::unordered_map<std::string, TopicState> m_topicStatesCache;
    mutable std::mutex m_topicStateCacheMutex;
};


#endif //FLOWBROKER_TOPICCACHE_HPP
