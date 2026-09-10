//
// Created by fran on 27/07/2026.
//

#ifndef FLOWBROKER_TOPICCACHE_HPP
#define FLOWBROKER_TOPICCACHE_HPP

#include <mutex>
#include <unordered_map>
#include "TopicState.hpp"
#include "../../common/TopicSnapshot.hpp"

/**
 * @class TopicCache
 * @brief The live state of every topic, shared across producer and consumer threads.
 *
 * Maps each topic name to its TopicState. 
 * The consumer thread writes here (as it processes incoming messages), while 
 * the server thread reads here (to answer TOPICS cmd or to build the dashboard).
 * All access goes through one mutex, so the cache is safe to use from several 
 * threads without the caller locking anything.
 *
 * Topics can be declared up front with addTopic(), so a topic exists and is
 * listed even before its first value arrives.
 */
class TopicCache {
public:
    TopicCache();

    /// Declare a topic with no data yet, so it is known before its first value.
    void addTopic(const TopicDescriptor &topicDescriptor);

    /// Record a sample for a topic, creating the topic if it is seen first here.
    void addTopicSample(const std::string &topicName, const std::vector<double> &values, const std::uint64_t timestampNs);
    
    /// Snapshot of one topic's current state (empty snapshot if unknown).
    TopicSnapshot getTopicSnapshot(const std::string &topicName) const;
    
    /// Snapshot of every topic's current state.
    std::vector<TopicSnapshot> getAllTopicsSnapshot() const;
    
    /// Name and type of every known topic (used to answer the TOPICS command).
    std::vector<TopicDescriptor> topics() const;

    bool hasTopic(const std::string &topicName) const;
    unsigned int topicId(const std::string &topicName) const;
    std::vector<std::string> topicSchema(const std::string &topicName) const;
    StreamType topicType(const std::string &topicName) const;
private:
    int m_nextTopicId;
    std::unordered_map<std::string, TopicState> m_topicStatesCache;
    mutable std::mutex m_topicStateCacheMutex; ///< Guards the map; mutable so const readers can lock.
};


#endif //FLOWBROKER_TOPICCACHE_HPP
