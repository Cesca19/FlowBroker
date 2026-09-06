//
// Created by fran on 04/09/2026.
//

#ifndef FLOWBROKER_TOPICDESCRIPTOR_HPP
#define FLOWBROKER_TOPICDESCRIPTOR_HPP

#include <string>
#include "Message.hpp"

struct TopicDescriptor {
    std::string topic;
    StreamType type;
};

#endif //FLOWBROKER_TOPICDESCRIPTOR_HPP
