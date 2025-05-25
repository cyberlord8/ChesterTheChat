#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QDateTime>

/**
 * @struct Message
 * @brief Represents a single chat message entry in the system.
 *
 * Used for both storing messages in the database and rendering them in the UI.
 */
struct Message {
    /**
     * @brief The name of the user who sent the message.
     */
    QString user;

    /**
     * @brief The message content.
     */
    QString text;

    /**
     * @brief UTC timestamp when the message was sent or received.
     */
    QDateTime timestamp;

    /**
     * @brief Indicates whether the message was sent by the local user.
     * True if sent by this client, false if received from another user.
     */
    bool isSentByMe = false;
};

#endif // STRUCTURES_H
