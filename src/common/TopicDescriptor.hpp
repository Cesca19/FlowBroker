//
// Created by fran on 04/09/2026.
//

#ifndef FLOWBROKER_TOPICDESCRIPTOR_HPP
#define FLOWBROKER_TOPICDESCRIPTOR_HPP

#include <string>
#include <vector>
#include "Message.hpp"

/**
 * @struct TopicDescriptor
 * @brief Everything known about a topic at declaration time, before any value.
 *
 * The schema is the ordered list of field names; its size equals the number of
 * values each message of this topic carries (position i in values[] is
 * schema[i]).
 */
struct TopicDescriptor {
    std::string name;
    StreamType type;
    std::vector<std::string> schema;  ///< Ordered field names, e.g. ["temp","wind","pressure"].
};

#endif //FLOWBROKER_TOPICDESCRIPTOR_HPP
