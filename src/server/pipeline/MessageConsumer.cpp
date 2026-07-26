//
// Created by fran on 26/07/2026.
//

#include "MessageConsumer.hpp"
#include <iostream>

MessageConsumer::MessageConsumer(MessageCatalog &catalog)
    : m_messageCatalog(catalog)
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

void MessageConsumer::processMessage(const Message &message)
{
    std::cout << "Message to process about topic: " << message.topic << std::endl;
}
