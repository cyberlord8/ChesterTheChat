#include "ChatFormatter.h"

#include "qapplication.h"
#include "qtextcursor.h"

ChatFormatter::ChatFormatter(QObject *parent)
    : QObject(parent)
{}

void ChatFormatter::insertBlock(QTextCursor &cursor, bool isSent)
{
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);
    cursor.insertBlock(blockFmt);
}//insertBlock

QColor ChatFormatter::resolveUserColor(const QString &user, bool isSent)
{
    if (isSent)
        return QColorConstants::Cyan;

    if (!userColorMap.contains(user)) {
        userColorMap.insert(user, generateColorForUser(user));
    }

    return userColorMap.value(user);
}//resolveUserColor

void ChatFormatter::insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color)
{
    QTextCharFormat fmt;
    fmt.setForeground(color);
    fmt.setFontWeight(QFont::Bold);
    fmt.setFontPointSize(11);
    cursor.insertText(user + "\n", fmt);
}//insertUserLine

void ChatFormatter::insertMessageLine(QTextCursor &cursor, const QString &message, bool isDark)
{
    QTextCharFormat fmt;
    fmt.setForeground(isDark ? Qt::white : Qt::black);
    fmt.setFontPointSize(14);
    cursor.insertText(message + "\n", fmt);
}//insertMessageLine

void ChatFormatter::insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, bool isDark)
{
    QTextCharFormat fmt;
    fmt.setForeground(isDark ? Qt::gray : Qt::darkGray);

    QFont f = baseFont;
    f.setPointSize(10);
    fmt.setFont(f);

    cursor.insertText(ts.toString("hh:mmZ") + "\n", fmt);
}//insertTimestampLine

void ChatFormatter::appendMessage(QTextEdit *textEdit, const QString &user,
                                  const QString &message, const QDateTime &timestamp,
                                  bool isDarkThemed, bool isSent)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    insertBlock(cursor, isSent);
    const QColor userColor = resolveUserColor(user, isSent);

    insertUserLine(cursor, user, userColor);
    insertMessageLine(cursor, message, isDarkThemed);
    insertTimestampLine(cursor, timestamp, textEdit->font(), isDarkThemed);

    textEdit->setTextCursor(cursor);
}//appendMessage

QColor ChatFormatter::generateColorForUser(const QString &user)
{
    // deterministic hash → HSV color
    QByteArray data = user.toUtf8();
    size_t hash = qHash(data);

    int hue = hash % 360;
    int sat = 180 + (hash / 360 % 75);
    int val = 200 + (hash / 10000 % 55);

    return QColor::fromHsv(hue, sat, val);
} //generateColorForUser
