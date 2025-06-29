/*
 * Chester The Chat
 * Copyright (C) 2024 Timothy Millea
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UDPCHATSOCKETMANAGER_H
#define UDPCHATSOCKETMANAGER_H

#include "../globals.h"

#include <QDateTime>
#include <QHostAddress>
#include <QObject>
#include <QUdpSocket>
/**
 * @class UdpChatSocketManager
 * @brief Manages sending and receiving of UDP chat messages.
 *
 * This class encapsulates all UDP networking operations for a chat application,
 * including socket binding, multicast group management, message sending, and
 * reception. It also filters out self-sent messages to prevent echo.
 */
class UdpChatSocketManager : public QObject
{
    Q_OBJECT

public:
    /**
 * @brief Checks if both send and receive sockets are currently active.
 * @return True if both sockets are valid and bound; false otherwise.
 */
    bool isConnected() const;

    /**
     * @brief Constructs the UDP chat socket manager.
     * @param parent The parent QObject.
     */
    explicit UdpChatSocketManager(QObject *parent = nullptr);

    /**
     * @brief Destructor. Closes and cleans up all active sockets.
     */
    ~UdpChatSocketManager();

    /**
     * @brief Binds the receive socket to a local and multicast group address.
     * @param localAddress The local interface address to bind.
     * @param groupAddress The multicast group address to join (if applicable).
     * @param port The port to listen on.
     * @return True if successfully bound, false on failure.
     */
    bool bindReceiveSocket(const QHostAddress &localAddress, const QHostAddress &groupAddress, quint16 port);

    /**
     * @brief Binds the send socket to a local address.
     * @param localAddress The local interface address to bind.
     * @return True if successfully bound, false otherwise.
     */
    bool bindSendSocket(const QHostAddress &localAddress);

    /**
     * @brief Sends a datagram to the specified target address and port.
     * @param data The message payload to send.
     * @param targetAddress The destination IP address.
     * @param targetPort The destination port number.
     * @return Number of bytes sent, or -1 on failure.
     */
    qint64 sendMessage(const QByteArray &data, const QHostAddress &targetAddress, quint16 targetPort);

    /**
     * @brief Closes both send and receive sockets and frees resources.
     */
    void closeSockets();

    /**
     * @brief Returns the last socket error as a human-readable string.
     * @return A QString describing the last error.
     */
    QString lastError() const;

signals:
    /**
     * @brief Emitted when a valid message is received.
     * @param user The sender's username.
     * @param message The message content.
     */
    void messageReceived(const QString &user, const QString &message);

private slots:
    /**
     * @brief Processes and handles incoming datagrams.
     * Filters out self-sent messages and emits messageReceived().
     */
    void processPendingDatagrams();

private:
    /** @brief UDP socket used for sending messages. */
    QUdpSocket *sendSocket = nullptr;

    /** @brief UDP socket used for receiving messages. */
    QUdpSocket *recvSocket = nullptr;

    /** @brief Stores the last sent datagram to detect self-echoes. */
    QByteArray lastSentData;

    /**
 * @brief Timestamp of the last sent datagram.
 *
 * Used in conjunction with lastSentData to detect and suppress self-echoed
 * UDP messages. A message identical to the last one sent is considered an
 * echo only if it is received within a short time window (e.g., 250 ms).
 */
    QDateTime lastSentTime;

    /** @brief Whether multicast sending is enabled. */
    bool multicastEnabled = false;

    /** @brief Whether loopback (self-message reception) is allowed. */
    bool loopbackEnabled = true;

    /**
     * @brief Safely deletes and nulls the receive socket.
     */
    void cleanupReceiveSocket();

    /**
     * @brief Creates and binds a receive socket to the specified address and port.
     * @param localAddress The local address to bind to.
     * @param port The UDP port to bind.
     * @return True on success, false on failure.
     */
    bool createAndBindReceiveSocket(const QHostAddress &localAddress, quint16 port);

    /**
     * @brief Joins a multicast group if the address is valid.
     * @param groupAddress The multicast address to join.
     */
    void joinMulticastGroupSafely(const QHostAddress &groupAddress);

    /**
     * @brief Closes and deletes the given socket safely.
     * @param socket Reference to the QUdpSocket pointer to cleanup.
     */
    void cleanupSocket(QUdpSocket *&socket);

    /**
     * @brief Receives a single datagram and extracts sender information.
     * @param sender Filled with the sender's address.
     * @param port Filled with the sender's port.
     * @return The raw datagram data.
     */
    QByteArray receiveDatagram(QHostAddress &sender, quint16 &port);

    /**
     * @brief Checks if a datagram is a self-sent echo.
     * @param datagram The received datagram.
     * @return True if it matches the last sent data, false otherwise.
     */
    bool isSelfEcho(const QByteArray &datagram) const;

    /**
     * @brief Parses a "user - message" formatted string.
     * @param raw The raw UTF-8 decoded message string.
     * @return A pair containing user and message.
     */
    std::pair<QString, QString> parseUserMessage(const QByteArray &raw) const;
};

#endif // UDPCHATSOCKETMANAGER_H
