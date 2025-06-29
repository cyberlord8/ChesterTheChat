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

#include "messagestore.h"

#include "../Utils/debugmacros.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

MessageStore::MessageStore(const QString &dbPath, int m_instanceID, QObject *parent)
    : QObject(parent)
    , m_connectionName(QString("chatdb_connection_%1").arg(m_instanceID))
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qCritical() << "[MessageStore] SQLite driver not available!";
        return;
    }

    db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(dbPath);

#ifdef DEBUG_MODE
    qDebug() << "[MessageStore] Initialized with DB path:" << dbPath << " and connection name:" << m_connectionName;
#endif
} //MessageStore

MessageStore::~MessageStore()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!QSqlDatabase::contains(m_connectionName))
        return;

    db = QSqlDatabase();

    {
        QSqlDatabase conn = QSqlDatabase::database(m_connectionName, /* open */ false);

        if (conn.isOpen())
            conn.close();
    }

    QSqlDatabase::removeDatabase(m_connectionName);
} //MessageStore

void MessageStore::logDatabaseOpenError() const
{
    LOG_DEBUG(Q_FUNC_INFO);

    qCritical() << "[MessageStore] Failed to open database:" << db.lastError().text();
}//logDatabaseOpenError

bool MessageStore::open()
{
    LOG_DEBUG(Q_FUNC_INFO);

    return initializeConnection() && initializeSchema();
}//open

bool MessageStore::initializeConnection()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!db.open()) {
        logDatabaseOpenError();
        return false;
    }
    return true;
}//initializeConnection

bool MessageStore::initializeSchema()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString createSql = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user TEXT NOT NULL,
            text TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            is_sent INTEGER DEFAULT 0
        )
    )";

    QSqlQuery query(conn());
    if (!query.exec(createSql)) {
        qCritical() << "[MessageStore] Failed to create 'messages' table:" << query.lastError().text();
        return false;
    }

    return true;
} //initializeSchema

//TODO - Maybe use this at a later date
// bool MessageStore::initializeSchemaWithVersioning()
// {
//     QSqlQuery query(conn());

//     // 1. Create meta table if it doesn't exist
//     if (!query.exec(R"(
//         CREATE TABLE IF NOT EXISTS meta (
//             key TEXT PRIMARY KEY,
//             value TEXT NOT NULL
//         )
//     )")) {
//         qCritical() << "[MessageStore] Failed to create 'meta' table:" << query.lastError().text();
//         return false;
//     }

//     // 2. Check schema version
//     QString schemaVersion = "0";
//     if (query.exec("SELECT value FROM meta WHERE key = 'schema_version'") && query.next()) {
//         schemaVersion = query.value(0).toString();
//     }

//     // 3. Apply schema based on version
//     if (schemaVersion == "0") {
//         // Initial schema setup
//         const QString createMessagesSql = R"(
//             CREATE TABLE IF NOT EXISTS messages (
//                 id INTEGER PRIMARY KEY AUTOINCREMENT,
//                 user TEXT NOT NULL,
//                 text TEXT NOT NULL,
//                 timestamp TEXT NOT NULL,
//                 is_sent INTEGER DEFAULT 0
//             )
//         )";

//         if (!query.exec(createMessagesSql)) {
//             qCritical() << "[MessageStore] Failed to create 'messages' table:" << query.lastError().text();
//             return false;
//         }

//         // Update schema version to 1
//         QSqlQuery updateVersion(conn());
//         updateVersion.prepare("INSERT OR REPLACE INTO meta (key, value) VALUES ('schema_version', '1')");
//         if (!updateVersion.exec()) {
//             qWarning() << "[MessageStore] Failed to set schema version:" << updateVersion.lastError().text();
//             return false;
//         }
//     }

//     // 4. Handle future migrations (example for version 2)
//     /*
//     if (schemaVersion == "1") {
//         // ALTER TABLE or other migration logic here
//         QSqlQuery alter;
//         alter.exec("ALTER TABLE messages ADD COLUMN message_type TEXT DEFAULT 'text'");

//         QSqlQuery updateVersion;
//         updateVersion.prepare("UPDATE meta SET value = '2' WHERE key = 'schema_version'");
//         updateVersion.exec();
//     }
//     */

//     return true;
// } // initializeSchemaWithVersioning

void MessageStore::insertMessage(const QString &user, const QString &text, const QDateTime &timestamp, bool isSent)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QSqlQuery query(conn());
    query.prepare(R"(
        INSERT INTO messages (user, text, timestamp, is_sent)
        VALUES (:user, :text, :timestamp, :is_sent)
    )");

    query.bindValue(":user", user);
    query.bindValue(":text", text);
    query.bindValue(":timestamp", timestamp.toString(Qt::ISODate));
    query.bindValue(":is_sent", isSent ? 1 : 0);

    if (!query.exec()) {
        qWarning().nospace() << "[MessageStore] Failed to insert message from '" << user << "': " << query.lastError().text();
    }
} //insertMessage

Message MessageStore::extractMessageFromQuery(const QSqlQuery &query) const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    Message m;
    m.user = query.value(0).toString();
    m.text = query.value(1).toString();
    m.timestamp = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
    m.isSentByMe = (query.value(3).toInt() == 1);
    return m;
} //extractMessageFromQuery

QList<Message> MessageStore::fetchLastMessages(int count)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QList<Message> messages;
    QSqlQuery query(conn());

    query.prepare(R"(
        SELECT user, text, timestamp, is_sent
        FROM messages
        ORDER BY id DESC
        LIMIT :limit
    )");
    query.bindValue(":limit", count);

    if (!query.exec()) {
        qWarning().nospace() << "[MessageStore] fetchLastMessages failed: " << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        messages.append(extractMessageFromQuery(query));
    }

    std::reverse(messages.begin(), messages.end()); // Return in chronological order
    return messages;
} //fetchLastMessages

QList<Message> MessageStore::fetchMessages(int offset, int limit)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QList<Message> messages;
    QSqlQuery query(conn());

    query.prepare(R"(
        SELECT user, text, timestamp, is_sent
        FROM messages
        ORDER BY id ASC
        LIMIT :limit OFFSET :offset
    )");

    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        qWarning().nospace() << "[MessageStore] fetchMessages failed: " << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        messages.append(extractMessageFromQuery(query));
    }

    return messages;
} //fetchMessages

int MessageStore::messageCount() const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QSqlQuery query("SELECT COUNT(*) FROM messages", conn());
    return query.next() ? query.value(0).toInt() : 0;
} //messageCount

bool MessageStore::clearMessages()
{
    LOG_DEBUG(Q_FUNC_INFO);

    QSqlQuery query("DELETE FROM messages", conn());
    if (!query.exec()) {
        qWarning() << "[MessageStore] Failed to clear messages:" << query.lastError().text();
        return false;
    }

    return true;
} //clearMessages
