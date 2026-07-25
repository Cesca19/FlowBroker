//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_MESSAGECATALOG_HPP
#define FLOWBROKER_MESSAGECATALOG_HPP

#include "../common/Message.hpp"
#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>

class MessageCatalog {
public:
    MessageCatalog();
    void push(const Message &message);
    std::optional<Message> pop();
private:
    std::queue<Message> m_messagesQueue;
    std::mutex m_messagesQueueMutex;
    std::condition_variable m_messagesQueueCondition;
};


#endif //FLOWBROKER_MESSAGECATALOG_HPP
