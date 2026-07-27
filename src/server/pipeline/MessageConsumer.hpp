//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_MESSAGECONSUMER_HPP
#define FLOWBROKER_MESSAGECONSUMER_HPP

#include "../topics/TopicCache.hpp"
#include "MessageCatalog.hpp"

class MessageConsumer {
public:
    MessageConsumer(MessageCatalog& catalog, TopicCache& topicCache);
    void run();
private:
    void processMessage(const Message& message) const;

    MessageCatalog& m_messageCatalog;
    TopicCache& m_topicCache;
};


#endif //FLOWBROKER_MESSAGECONSUMER_HPP
