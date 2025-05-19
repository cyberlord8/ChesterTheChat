#ifndef UDPCHATSOCKETMANAGER_H
#define UDPCHATSOCKETMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

/**
 * @class UdpChatSocketManager
 * @brief Manages sending and receiving of UDP chat messages.
 *
 * This class encapsulates all UDP networking operations for a chat application,
 * including socket binding, multicast group management, message sending, and
 * reception. It also filters out self-sent messages to prevent echo.
 */
class UdpChatSocketManager : public QObject {
    Q_OBJECT

public:
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
     * @brief Enables or disables loopback mode (self-receiving).
     * @param enabled True to allow loopback, false to disable.
     */
    void setLoopbackMode(bool enabled);

    /**
     * @brief Enables or disables multicast transmission mode.
     * @param enabled True to enable multicast, false otherwise.
     */
    void setMulticastMode(bool enabled);

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
    std::pair<QString, QString> parseUserMessage(const QString &raw) const;
};


#endif // UDPCHATSOCKETMANAGER_H
