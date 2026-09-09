//
// Created by fran on 25/07/2026.
//

#ifndef FLOWBROKER_MESSAGECATALOG_HPP
#define FLOWBROKER_MESSAGECATALOG_HPP

#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>
#include "../../common/Message.hpp"

/**
 * @class MessageCatalog
 * @brief Thread-safe queue between the producer and the consumer of messages.
 *
 * The producer side pushes messages in, the consumer thread pops them out. The
 * two run on different threads, so every access is guarded by one mutex, and a
 * condition variable lets the consumer sleep while the queue is empty instead of
 * spinning (no busy-wait).
 *
 * Shutdown works through the same condition variable: stop() flips a flag and
 * wakes the consumer, and pop() then returns an empty optional to signal that
 * there are no more messages to process.
 */
class MessageCatalog {
public:
    MessageCatalog();

    /// Push a message onto the queue and wake one waiting consumer.
    void push(const Message &message);

    /**
     * @brief Take the next message, waiting if the queue is empty.
     * @return The next message, or an empty optional if the catalog is stopping.
     */
    std::optional<Message> pop();

    /**
     * @brief Signal shutdown: wake any waiting consumer so it can leave its loop.
     */
    void stop();
private:
    ///  It has a Wake condition: something to consume, or we are stopping.
    bool shouldWakeUp() const;

    bool m_isStopping;                             ///< Set by stop(), checked on wake.
    std::queue<Message> m_messagesQueue;           ///< The pending messages.
    std::mutex m_messagesQueueMutex;               ///< Guards the queue and the flag.
    std::condition_variable m_messagesQueueCondition; ///< Lets the consumer sleep until woken.
};


#endif //FLOWBROKER_MESSAGECATALOG_HPP
