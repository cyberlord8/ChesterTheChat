#ifndef MESSAGESTORE_H
#define MESSAGESTORE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QList>
#include "globals.h"

/**
 * @class MessageStore
 * @brief Handles storage and retrieval of chat messages using an SQLite database.
 *
 * MessageStore provides an interface to insert, fetch, and manage chat messages in a persistent store.
 * It supports paged message access, initialization of the schema, and clearing stored data.
 */
class MessageStore : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a MessageStore object with a specified database path.
     * @param dbPath Path to the SQLite database file.
     * @param parent The parent QObject.
     */
    explicit MessageStore(const QString &dbPath, const int m_instanceID, QObject *parent = nullptr);
    ~MessageStore();
    /**
     * @brief Opens the SQLite database and initializes the message schema if necessary.
     * @return True if the database was successfully opened and initialized, false otherwise.
     */
    bool open();

    /**
     * @brief Inserts a new message into the database.
     * @param user The name of the message sender.
     * @param text The content of the message.
     * @param timestamp The timestamp of the message.
     * @param isSent Indicates whether the message was sent by the local user.
     */
    void insertMessage(const QString &user, const QString &text, const QDateTime &timestamp, bool isSent);

    /**
     * @brief Fetches the most recent messages from the database.
     * @param count The maximum number of messages to retrieve.
     * @return A list of Message objects in chronological order.
     */
    QList<Message> fetchLastMessages(int count);

    /**
     * @brief Fetches a specific range of messages using offset and limit.
     * @param offset The starting position of the records to fetch.
     * @param limit The maximum number of records to fetch.
     * @return A list of Message objects in ascending order by ID.
     */
    QList<Message> fetchMessages(int offset, int limit);

    /**
     * @brief Returns the total number of messages stored in the database.
     * @return The total message count.
     */
    int messageCount() const;

    /**
     * @brief Deletes all messages from the database.
     * @return True if the operation was successful, false otherwise.
     */
    bool clearMessages();

private:
    /**
     * @brief Initializes the database schema if it doesn't exist.
     * @return True if schema initialization succeeded, false otherwise.
     */
    bool initializeSchema();

    /**
 * @brief Retrieves the QSqlDatabase connection associated with this instance.
 *
 * Ensures that all SQL queries operate on the correct named connection created
 * during initialization. This avoids conflicts with the default connection and
 * is critical for multi-instance or embedded SQLite setups.
 *
 * @return The QSqlDatabase object bound to this message store's connection name.
 */
    QSqlDatabase conn() const { return QSqlDatabase::database(m_connectionName); }

    /**
     * @brief Logs database connection errors for debugging.
     */
    void logDatabaseOpenError() const;

    /**
     * @brief Extracts a Message object from a query result row.
     * @param query The current row in the executed query.
     * @return The extracted Message.
     */
    Message extractMessageFromQuery(const QSqlQuery &query) const;

    /**
     * @brief The internal database instance.
     */
    QSqlDatabase db;

    /**
 * @brief Unique name used for the SQLite database connection.
 *
 * Ensures that each instance of MessageStore uses a distinct connection
 * to avoid conflicts or reuse within the QSqlDatabase connection pool.
 * Used to identify and later remove the connection cleanly on destruction.
 */
    QString m_connectionName;

    /**
 * @brief Initializes the database schema with version tracking support.
 *
 * Creates required tables including a `meta` table to track the schema version.
 * If no version is present, it initializes the `messages` table and sets version to 1.
 * Future schema migrations can be handled based on the current version value stored in `meta`.
 *
 * @return True if schema initialization or upgrade was successful, false otherwise.
 * @note NOT CURRENTLY USED - For possible future expansion
 */
    bool initializeSchemaWithVersioning();
};

#endif // MESSAGESTORE_H
