//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_MESSAGECONSUMER_HPP
#define FLOWBROKER_MESSAGECONSUMER_HPP

#include "../topics/TopicCache.hpp"
#include "MessageCatalog.hpp"

/**
 * @class MessageConsumer
 * @brief Drains the catalog and updates the topic cache. Runs on its own thread.
 *
 * Sits between the two shared structures: it pops messages from the
 * MessageCatalog and writes their values into the TopicCache. run() is the loop
 * that thread executes; it ends when the catalog signals shutdown.
 */
class MessageConsumer {
public:
    MessageConsumer(MessageCatalog& catalog, TopicCache& topicCache);

    /// It pops a message and processes it, until the catalog signals shutdown.
    void run() const;
private:
    /// Record one message's value into the cache (skips empty-value messages).
    void processMessage(const Message& message) const;

    MessageCatalog& m_messageCatalog;  ///< Source of messages (popped from).
    TopicCache& m_topicCache;          ///< Destination of results (written to).
};


#endif //FLOWBROKER_MESSAGECONSUMER_HPP
