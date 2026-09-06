//
// Created by fran on 04/09/2026.
//

#include "NetworkUtils.hpp"

std::optional<std::uint16_t> parsePort(const std::string_view text)
{
    int value = 0;
    const char *begin = text.data();
    const char *end = text.data() + text.size();

    auto [ptr, ec] = std::from_chars(begin, end, value);
    // Reject if parsing failed, or if characters remain after the number.
    if (ec != std::errc() || ptr != end)
        return std::nullopt;
    if (value < network::minPort || value > network::maxPort)
        return std::nullopt;
    return static_cast<std::uint16_t>(value);
}


std::vector<std::string> splitString(const std::string &message, const char delimiter)
{
    std::vector<std::string> result;
    std::istringstream ss(message);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}
