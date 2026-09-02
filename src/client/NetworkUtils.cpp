//
// Created by fran on 01/09/2026.
//

#include "NetworkUtils.hpp"
#include <QHostAddress>

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

std::optional<std::string> parseHost(const std::string &text)
{
    QHostAddress address;
    if (!address.setAddress(QString::fromStdString(text)))
        return std::nullopt;
    return text;
}
