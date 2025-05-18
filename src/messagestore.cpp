
#include "messagestore.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

MessageStore::MessageStore(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
}

bool MessageStore::open()
{
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    initializeSchema();
    return true;
}

void MessageStore::initializeSchema()
{
    QSqlQuery query;
    const QString createSql = R"(
    CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user TEXT NOT NULL,
        text TEXT NOT NULL,
        timestamp TEXT NOT NULL,
        is_sent INTEGER DEFAULT 0
    )
)";


    if (!query.exec(createSql)) {
        qCritical() << "Failed to create messages table:" << query.lastError().text();
    }
}

void MessageStore::insertMessage(const QString &user, const QString &text, const QDateTime &timestamp, bool isSent)
{
    QSqlQuery query;
    query.prepare("INSERT INTO messages (user, text, timestamp, is_sent) VALUES (?, ?, ?, ?)");
    query.addBindValue(user);
    query.addBindValue(text);
    query.addBindValue(timestamp.toString(Qt::ISODate));
    query.addBindValue(isSent ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Failed to insert message:" << query.lastError().text();
    }
}

QList<Message> MessageStore::fetchLastMessages(int count)
{
    QList<Message> messages;
    QSqlQuery query;

    query.prepare(R"(
        SELECT user, text, timestamp, is_sent
        FROM messages
        ORDER BY id DESC
        LIMIT ?
    )");
    query.addBindValue(count);

    if (query.exec()) {
        while (query.next()) {
            Message m;
            m.user = query.value(0).toString();
            m.text = query.value(1).toString();
            m.timestamp = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
            m.isSentByMe = query.value(3).toInt() == 1;
            messages.append(m);
        }
        std::reverse(messages.begin(), messages.end());  // Oldest to newest
    } else {
        qWarning() << "fetchLastMessages failed:" << query.lastError().text();
    }

    return messages;
}//

QList<Message> MessageStore::fetchMessages(int offset, int limit)
{
    QList<Message> messages;
    QSqlQuery query;
    query.prepare("SELECT user, text, timestamp, is_sent FROM messages ORDER BY id ASC LIMIT ? OFFSET ?");
    query.addBindValue(limit);
    query.addBindValue(offset);

    if (query.exec()) {
        while (query.next()) {
            Message m;
            m.user = query.value(0).toString();
            m.text = query.value(1).toString();
            m.timestamp = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
            m.isSentByMe = query.value(3).toInt() == 1;
            messages.append(m);
        }
    } else {
        qWarning() << "Failed to fetch messages:" << query.lastError().text();
    }

    return messages;
}//fetchMessages


int MessageStore::messageCount() const
{
    QSqlQuery query("SELECT COUNT(*) FROM messages");
    return query.next() ? query.value(0).toInt() : 0;
}
