//
// Created by fran on 01/09/2026.
//

#ifndef FLOWBROKER_NETWORKUTILS_HPP
#define FLOWBROKER_NETWORKUTILS_HPP

/**
 * @file NetworkUtils.hpp
 * @brief Validation of the connection settings typed by the user.
 */

#include <charconv>
#include <cstdint>
#include <string>
#include <optional>
#include "../common/NetworkDefaults.hpp"

/// Validate an IPv4 or IPv6 literal, returning it unchanged when it parses.
std::optional<std::string> parseHost(const std::string &text);

/// Parse a full port number, rejecting trailing characters and out of range values.
std::optional<std::uint16_t> parsePort(const std::string_view text);

#endif //FLOWBROKER_NETWORKUTILS_HPP
