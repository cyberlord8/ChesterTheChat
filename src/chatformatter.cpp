#include "ChatFormatter.h"
#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QColor>
#include <QPalette>
#include <QDateTime>
#include <QtGlobal>   // for qGray
#include <QCryptographicHash>

ChatFormatter::ChatFormatter(QObject *parent)
    : QObject(parent)
{}

void ChatFormatter::appendMessage(QTextEdit *textEdit,
                                  const QString &user,
                                  const QString &message,
                                  const QDateTime &timestamp,
                                  bool isSent)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    // Alignment
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);
    cursor.insertBlock(blockFmt);

    // User-color (sent always cyan; received get their own color)
    QColor userColor = isSent
                           ? QColorConstants::Cyan
                           : userColorMap.value(user);
    if (!isSent && !userColorMap.contains(user)) {
        userColor = generateColorForUser(user);
        userColorMap.insert(user, userColor);
    }

    // User line
    {
        QTextCharFormat fmt;
        fmt.setForeground(userColor);
        fmt.setFontWeight(QFont::Bold);
        cursor.insertText(user + "\n", fmt);
    }

    // Message line
    {
        QTextCharFormat fmt;
        fmt.setForeground(isDarkTheme(textEdit) ? Qt::white : Qt::black);
        cursor.insertText(message + "\n", fmt);
    }

    // Timestamp line
    {
        QTextCharFormat fmt;
        fmt.setForeground(isDarkTheme(textEdit) ? Qt::gray : Qt::darkGray);
        QFont f = textEdit->font();
        f.setPointSize(8);
        fmt.setFont(f);
        cursor.insertText(timestamp.toString("hh:mm") + "\n", fmt);
    }

    textEdit->setTextCursor(cursor);
}

QColor ChatFormatter::generateColorForUser(const QString &user)
{
    // deterministic hash → HSV color
    QByteArray data = user.toUtf8();
    size_t hash = qHash(data);
    int hue = hash % 360;
    int sat = 180 + (hash / 360 % 75);
    int val = 200 + (hash / 10000 % 55);
    return QColor::fromHsv(hue, sat, val);
}

bool ChatFormatter::isDarkTheme(const QTextEdit *textEdit) const
{
    QColor bg = textEdit->palette().color(QPalette::Base);
    return qGray(bg.rgb()) < 128;
}
