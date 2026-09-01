//
// Created by fran on 01/09/2026.
//

#ifndef FLOWBROKER_NETWORKDEFAULTS_HPP
#define FLOWBROKER_NETWORKDEFAULTS_HPP

#include <cstdint>
#include <string_view>

// Shared network default values, included by both the server and the client so the
namespace network {
    inline constexpr std::string_view defaultHost = "127.0.0.1";
    inline constexpr std::uint16_t defaultTcpPort = 6666;
    inline constexpr std::uint16_t defaultUdpPort = 7777;
    inline constexpr int minPort = 1024;
    inline constexpr int maxPort = 65535;

}

#endif //FLOWBROKER_NETWORKDEFAULTS_HPP
