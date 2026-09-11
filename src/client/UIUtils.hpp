//
// Created by fran on 02/09/2026.
//

#ifndef FLOWBROKER_UIUTILS_HPP
#define FLOWBROKER_UIUTILS_HPP


/**
 * @enum MessageType
 * @brief Severity of a message shown to the user.
 *
 */
enum class MessageType {
    Info,
    Warning,
    Error,
};


/**
 * @enum ConnectionState
 * @brief High level state of the TCP control connection.
 *
 * Simplified view of QAbstractSocket::SocketState. The UI only needs to know
 * whether the connection form stays editable and what the connect button should show. 
 */
enum class ConnectionState {
    Disconnected,
    Closing,
    Connecting,
    Connected,
};

#endif //FLOWBROKER_UIUTILS_HPP
