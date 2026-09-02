//
// Created by fran on 02/09/2026.
//

#ifndef FLOWBROKER_UIUTILS_HPP
#define FLOWBROKER_UIUTILS_HPP

enum class MessageType {
    Info,
    Warning,
    Error,
};

enum class ConnectionState {
    Disconnected,
    Closing,
    Connecting,
    Connected,
};

#endif //FLOWBROKER_UIUTILS_HPP
