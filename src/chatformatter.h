#ifndef CHATFORMATTER_H
#define CHATFORMATTER_H

#include <QObject>
#include <QMap>
#include <QColor>
#include <QString>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCryptographicHash>
#include <QPalette>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QtGlobal> // for qGray

#include "settingsmanager.h"

class QTextEdit;

class ChatFormatter : public QObject
{
    Q_OBJECT
public:
    explicit ChatFormatter(QObject *parent = nullptr);

    void appendMessage(QTextEdit *textEdit, const QString &user, const QString &message, const QDateTime &timestamp, bool isDarkThemed, bool isSent);

private:
    QColor generateColorForUser(const QString &user);
    QMap<QString, QColor> userColorMap;
    void insertBlock(QTextCursor &cursor, bool isSent);
    QColor resolveUserColor(const QString &user, bool isSent);
    void insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color);
    void insertMessageLine(QTextCursor &cursor, const QString &message, bool isDark);
    void insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, bool isDark);
};

#endif // CHATFORMATTER_H
