//
// Created by fran on 19/07/2026.
//

#ifndef FLOWBROKER_MESSAGE_HPP
#define FLOWBROKER_MESSAGE_HPP

#include <cstdint>
#include <string>
#include <vector>

// How a client should INTERPRET the values (how to display / label them).
enum class StreamType : std::uint16_t {
    FINANCE,
    WEATHER,
    SENSOR,
};

// The common in-memory envelope. Every data source produces this, whatever its origin
// (GBM simulation, finance API, weather API, sensors). The server routes it by
// `topic` alone and never inspects the contents: this is what makes the broker
// domain-agnostic.
struct Message {
    StreamType          type;        // Finance / Weather / Sensor
    std::string         topic;       // "S&P", "Paris", "sensor-A"
    std::vector<double> values;      // payload: [price] or [temp, wind, pressure]
    std::uint64_t       timestampNs; // when the data was emitted, in nanoseconds
};

#endif //FLOWBROKER_MESSAGE_HPP
