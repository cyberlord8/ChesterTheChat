#include "ChatFormatter.h"
#include <QColor>
#include <QCryptographicHash>
#include <QDateTime>
#include <QPalette>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QtGlobal> // for qGray
#include "qapplication.h"

ChatFormatter::ChatFormatter(QObject *parent)
    : QObject(parent)
{}

void ChatFormatter::appendMessage(QTextEdit *textEdit, const QString &user,
                                  const QString &message, const QDateTime &timestamp,
                                  bool isDarkThemed, bool isSent)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    // Alignment
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);
    cursor.insertBlock(blockFmt);

    // User-color (sent always cyan; received get their own color)
    QColor userColor = isSent ? QColorConstants::Cyan : userColorMap.value(user);
    if (!isSent && !userColorMap.contains(user)) {
        userColor = generateColorForUser(user);
        userColorMap.insert(user, userColor);
    }

    // User line
    {
        QTextCharFormat fmt;
        fmt.setForeground(userColor);
        fmt.setFontWeight(QFont::Bold);
        fmt.setFontPointSize(11);
        cursor.insertText(user + "\n", fmt);
    }


    // // Message line (in a bubble)
    // {
    //     QString bubbleHtml = QString("<span style='background-color:%1; "
    //                                  "color:black; "
    //                                  "padding:40px 40px; "
    //                                  "border-radius:10px; "
    //                                  "font-size: 14pt; "
    //                                  "display:inline-block;'>"
    //                                  "%2"
    //                                  "</span><br>")
    //                              .arg(userColor.name(), message.toHtmlEscaped());

    //     cursor.insertHtml(bubbleHtml);
    // }

    // Message line
    {
        QTextCharFormat fmt;
        fmt.setForeground(isDarkThemed ? Qt::white : Qt::black);
        fmt.setFontPointSize(14);
        cursor.insertText(message + "\n", fmt);
    }

    // Timestamp line
    {
        QTextCharFormat fmt;
        fmt.setForeground(isDarkThemed ? Qt::gray : Qt::darkGray);
        QFont f = textEdit->font();
        f.setPointSize(10);
        fmt.setFont(f);
        cursor.insertText(timestamp.toString("hh:mmZ") + "\n", fmt);
    }

    textEdit->setTextCursor(cursor);
} //

QColor ChatFormatter::generateColorForUser(const QString &user)
{
    // deterministic hash → HSV color
    QByteArray data = user.toUtf8();
    size_t hash = qHash(data);
    int hue = hash % 360;
    int sat = 180 + (hash / 360 % 75);
    int val = 200 + (hash / 10000 % 55);
    return QColor::fromHsv(hue, sat, val);
} //
