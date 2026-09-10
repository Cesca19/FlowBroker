//
// Created by fran on 10/09/2026.
//

#ifndef FLOWBROKER_ALERT_HPP
#define FLOWBROKER_ALERT_HPP

#include <string>

struct Alert {
    unsigned int id;
    std::string topic;
    std::string field;
    std::string op;      // ">", "<", ">=", "<=" , "=="
    double threshold;
    double lastValue;  // Last value that triggered the alert
    bool isTriggered = false;
};

#endif //FLOWBROKER_ALERT_HPP