
#include "messagestore.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

MessageStore::MessageStore(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qCritical() << "[MessageStore] SQLite driver not available!";
        return;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

#ifdef DEBUG_MODE
    qDebug() << "[MessageStore] Initialized with DB path:" << dbPath;
#endif
}//MessageStore

void MessageStore::logDatabaseOpenError() const
{
    qCritical() << "[MessageStore] Failed to open database:" << db.lastError().text();
}

bool MessageStore::open()
{
    if (!db.open()) {
        logDatabaseOpenError();
        return false;
    }

    return initializeSchema();
}//open

bool MessageStore::initializeSchema()
{
    const QString createSql = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user TEXT NOT NULL,
            text TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            is_sent INTEGER DEFAULT 0
        )
    )";

    QSqlQuery query;
    if (!query.exec(createSql)) {
        qCritical() << "[MessageStore] Failed to create 'messages' table:" << query.lastError().text();
        return false;
    }

#ifdef DEBUG_MODE
    qDebug() << "[MessageStore] Database schema initialized successfully.";
#endif

    return true;
}//initializeSchema

void MessageStore::insertMessage(const QString &user, const QString &text, const QDateTime &timestamp, bool isSent)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO messages (user, text, timestamp, is_sent)
        VALUES (:user, :text, :timestamp, :is_sent)
    )");

    query.bindValue(":user", user);
    query.bindValue(":text", text);
    query.bindValue(":timestamp", timestamp.toString(Qt::ISODate));
    query.bindValue(":is_sent", isSent ? 1 : 0);

    if (!query.exec()) {
        qWarning().nospace()
        << "[MessageStore] Failed to insert message from '" << user
        << "': " << query.lastError().text();
    }
}//insertMessage

Message MessageStore::extractMessageFromQuery(const QSqlQuery &query) const
{
    Message m;
    m.user = query.value(0).toString();
    m.text = query.value(1).toString();
    m.timestamp = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
    m.isSentByMe = (query.value(3).toInt() == 1);
    return m;
}//extractMessageFromQuery

QList<Message> MessageStore::fetchLastMessages(int count)
{
    QList<Message> messages;
    QSqlQuery query;

    query.prepare(R"(
        SELECT user, text, timestamp, is_sent
        FROM messages
        ORDER BY id DESC
        LIMIT :limit
    )");
    query.bindValue(":limit", count);

    if (!query.exec()) {
        qWarning().nospace() << "[MessageStore] fetchLastMessages failed: "
                             << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        messages.append(extractMessageFromQuery(query));
    }

    std::reverse(messages.begin(), messages.end());  // Return in chronological order
    return messages;
}//fetchLastMessages

QList<Message> MessageStore::fetchMessages(int offset, int limit)
{
    QList<Message> messages;
    QSqlQuery query;

    query.prepare(R"(
        SELECT user, text, timestamp, is_sent
        FROM messages
        ORDER BY id ASC
        LIMIT :limit OFFSET :offset
    )");

    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        qWarning().nospace() << "[MessageStore] fetchMessages failed: "
                             << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        messages.append(extractMessageFromQuery(query));
    }

    return messages;
}//fetchMessages

int MessageStore::messageCount() const
{
    QSqlQuery query("SELECT COUNT(*) FROM messages");
    return query.next() ? query.value(0).toInt() : 0;
}//messageCount
