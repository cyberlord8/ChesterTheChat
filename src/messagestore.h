#ifndef MESSAGESTORE_H
#define MESSAGESTORE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QList>
#include "globals.h"

class MessageStore : public QObject {
    Q_OBJECT

public:
    explicit MessageStore(const QString &dbPath, QObject *parent = nullptr);
    QList<Message> fetchLastMessages(int count);
    bool open();
    void insertMessage(const QString &user, const QString &text, const QDateTime &timestamp, bool isSent);
    QList<Message> fetchMessages(int offset, int limit);
    int messageCount() const;

private:
    void initializeSchema();
    QSqlDatabase db;
};

#endif // MESSAGESTORE_H
