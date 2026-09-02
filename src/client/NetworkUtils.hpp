//
// Created by fran on 01/09/2026.
//

#ifndef FLOWBROKER_NETWORKUTILS_HPP
#define FLOWBROKER_NETWORKUTILS_HPP

#include <charconv>
#include <cstdint>
#include <string>
#include <optional>
#include "../common/NetworkDefaults.hpp"

std::optional<std::string> parseHost(const std::string &text);
std::optional<std::uint16_t> parsePort(const std::string_view text);

#endif //FLOWBROKER_NETWORKUTILS_HPP
