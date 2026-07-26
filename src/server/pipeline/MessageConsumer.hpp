//
// Created by fran on 26/07/2026.
//

#ifndef FLOWBROKER_MESSAGECONSUMER_HPP
#define FLOWBROKER_MESSAGECONSUMER_HPP

#include "MessageCatalog.hpp"

class MessageConsumer {
public:
    MessageConsumer(MessageCatalog& catalog);
    void run();
private:
    void processMessage(const Message& message);

    MessageCatalog& m_messageCatalog;
};


#endif //FLOWBROKER_MESSAGECONSUMER_HPP
