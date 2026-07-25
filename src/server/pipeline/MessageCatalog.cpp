//
// Created by fran on 25/07/2026.
//

#include "MessageCatalog.hpp"

MessageCatalog::MessageCatalog()
{
}

void MessageCatalog::push(const Message &message)
{
    m_messagesQueue.push(message);
}

std::optional<Message> MessageCatalog::pop()
{
    if (m_messagesQueue.empty())
        return {};
    Message message = m_messagesQueue.front();
    m_messagesQueue.pop();
    return message;
}


