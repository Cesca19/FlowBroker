//
// Created by fran on 04/09/2026.
//

#ifndef FLOWBROKER_NETWORKUTILS_HPP
#define FLOWBROKER_NETWORKUTILS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <charconv>
#include <vector>
#include "../../common/NetworkDefaults.hpp"


std::optional<std::uint16_t> parsePort(const std::string_view text);
std::vector<std::string> splitString(const std::string &message, char delimiter);


#endif //FLOWBROKER_NETWORKUTILS_HPP
