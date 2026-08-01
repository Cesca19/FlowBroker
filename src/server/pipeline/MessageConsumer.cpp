//
// Created by fran on 26/07/2026.
//

#include "MessageConsumer.hpp"

MessageConsumer::MessageConsumer(MessageCatalog &catalog, TopicCache& topicCache)
    : m_messageCatalog(catalog)
    , m_topicCache(topicCache)
{
}

void MessageConsumer::run()
{
    while (true) {
        const std::optional<Message> newMessage = m_messageCatalog.pop();
        if (!newMessage) {
            break;   // empty optional means shutdown: leave the loop, the thread ends
        }
        processMessage(newMessage.value());
    }
}

void MessageConsumer::processMessage(const Message &message) const
{
    if (message.values.empty())
        return; // nothing to record
    // for now we only handle messages with one value
    // later we migt handle the cache like topic-value0 topic-value1 etc so we will have multiple topic states by Topic
    m_topicCache.addTopicSample(message.topic, message.values[0], message.timestampNs);
}
