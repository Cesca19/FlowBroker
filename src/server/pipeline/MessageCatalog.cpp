//
// Created by fran on 25/07/2026.
//

#include "MessageCatalog.hpp"

MessageCatalog::MessageCatalog()
    : m_isStopping(false)
{
}

void MessageCatalog::push(const Message &message)
{
    {
        std::lock_guard<std::mutex> lock_guard(m_messagesQueueMutex);
        m_messagesQueue.push(message);
    }
    m_messagesQueueCondition.notify_one();
}

std::optional<Message> MessageCatalog::pop()
{
    std::unique_lock<std::mutex> uniqueLock(m_messagesQueueMutex);

    m_messagesQueueCondition.wait(uniqueLock, [this] () { return shouldWakeUp(); });
    if (!m_messagesQueue.empty()) {
        Message message = m_messagesQueue.front();
        m_messagesQueue.pop();
        return message;
    }
    return {};
}

void MessageCatalog::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_messagesQueueMutex);
        m_isStopping = true;
    }
    m_messagesQueueCondition.notify_all();
}

bool MessageCatalog::shouldWakeUp() const
{
    return !m_messagesQueue.empty() || m_isStopping;
}


