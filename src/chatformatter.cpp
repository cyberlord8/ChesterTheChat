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

#include "chatformatter.h"
#include "src/debugmacros.h"

#include <QTextEdit>

ChatFormatter::ChatFormatter(QObject *parent)
    : QObject(parent)
{
    LOG_DEBUG(Q_FUNC_INFO);
}

int ChatFormatter::calculateDynamicMargin(QTextCursor &cursor, double percent, int fallback) const
{
    QTextEdit *edit = qobject_cast<QTextEdit *>(cursor.document()->parent());
    int editorWidth = edit ? edit->viewport()->width() : fallback;
    return static_cast<int>(editorWidth * percent);
} //calculateDynamicMargin

void ChatFormatter::insertBlock(QTextCursor &cursor, bool isSent)
{
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);

    int margin = calculateDynamicMargin(cursor, 0.25, 600);

    if (isSent) {
        blockFmt.setLeftMargin(margin);
        blockFmt.setRightMargin(10);
    } else {
        blockFmt.setRightMargin(margin);
    }

    cursor.insertBlock(blockFmt);
} //insertBlock

QColor ChatFormatter::generateUserColor(const QString &user)
{
    if (!userColorMap.contains(user)) {
        userColorMap.insert(user, generateColorForUser(user));
    }
    return userColorMap.value(user);
}

QColor ChatFormatter::resolveUserColor(const QString &user, bool isSent)
{
    return isSent ? QColorConstants::Cyan : generateUserColor(user);
}

void ChatFormatter::insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color)
{
    QTextCharFormat fmt;
    fmt.setForeground(color);
    fmt.setFontWeight(QFont::Bold);
    fmt.setFontPointSize(11);

    cursor.insertText("\n" + user + "\n", fmt);
} //insertUserLine

void ChatFormatter::insertMessageLine(QTextCursor &cursor, const QString &message)
{
    QTextCharFormat fmt;
    // fmt.setForeground(isDark ? Qt::white : Qt::black);
    fmt.setFontPointSize(14);

    cursor.insertText(message + "\n", fmt);
} //insertMessageLine

void ChatFormatter::insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, const bool &isDark)
{
    QTextCharFormat fmt;
    fmt.setForeground(isDark ? Qt::gray : Qt::darkGray);

    QFont f = baseFont;
    f.setPointSize(10);
    fmt.setFont(f);

    cursor.insertText(ts.toString("hh:mmZ"), fmt);
} //insertTimestampLine

void ChatFormatter::appendMessage(QTextEdit *textEdit, const QString &user, const QString &message, const QDateTime &timestamp, bool isSent)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    const QColor userColor = resolveUserColor(user, isSent);

    if (!user.isEmpty()) {
        insertBlock(cursor, isSent);
        insertUserLine(cursor, user, userColor);
        insertMessageLine(cursor, message);
    } else {
        insertMessageLine(cursor, "\n" + message);
    }

    QString styleSheet = textEdit->styleSheet();

    bool isDark = styleSheet.simplified().contains("QTextEdit#textEditChat { color: black;}");

    insertTimestampLine(cursor, timestamp, textEdit->font(), isDark);

    textEdit->setTextCursor(cursor);
}//appendMessage

void ChatFormatter::insertLastReadMarker(QTextEdit *textEdit)
{
    if (!textEdit)
        return;

    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(Qt::AlignHCenter);
    cursor.insertBlock(blockFmt);

    QTextCharFormat charFmt;
    charFmt.setForeground(Qt::red);
    charFmt.setFontItalic(true);
    charFmt.setFontPointSize(10);

    cursor.insertText("──────────── Last Read ────────────\n", charFmt);
    textEdit->setTextCursor(cursor);
}//insertLastReadMarker

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
