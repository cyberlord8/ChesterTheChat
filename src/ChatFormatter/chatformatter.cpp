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

#include "ChatFormatter.h"

#include "qapplication.h"
#include "qtextcursor.h"

#include "../Utils/debugmacros.h"

ChatFormatter::ChatFormatter(QObject *parent)
    : QObject(parent)
{
    LOG_DEBUG(Q_FUNC_INFO);

}

int ChatFormatter::calculateDynamicMargin(QTextEdit *textEdit, double percent, int fallback) const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    if (!textEdit || !textEdit->viewport())
        return fallback;

    int editorWidth = textEdit ? textEdit->viewport()->width() : fallback;
    return static_cast<int>(editorWidth * percent);
} //calculateDynamicMargin

void ChatFormatter::insertBlock(QTextEdit * textEdit, QTextCursor &cursor, bool isSent)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(isSent ? Qt::AlignRight : Qt::AlignLeft);

    int margin = calculateDynamicMargin(textEdit, BORDER_MARGIN, 600);

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
    // LOG_DEBUG(Q_FUNC_INFO);

    if (!userColorMap.contains(user)) {
        userColorMap.insert(user, generateColorForUser(user));
    }
    return userColorMap.value(user);
}

QColor ChatFormatter::resolveUserColor(const QString &user, bool isSent)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    return isSent ? QColorConstants::Cyan : generateUserColor(user);
}

void ChatFormatter::insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QTextCharFormat fmt;
    fmt.setForeground(color);
    fmt.setFontWeight(QFont::Bold);
    fmt.setFontPointSize(11);

    cursor.insertText("\n" + user + "\n", fmt);
} //insertUserLine

void ChatFormatter::insertMessageLine(QTextCursor &cursor, const QString &message)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QTextCharFormat fmt;
    fmt.setFontPointSize(14);

    cursor.insertText(message + "\n", fmt);
} //insertMessageLine

void ChatFormatter::insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, const bool &isDark)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QTextCharFormat fmt;
    fmt.setForeground(isDark ? Qt::gray : Qt::darkGray);

    QFont f = baseFont;
    f.setPointSize(10);
    fmt.setFont(f);

    cursor.insertText(ts.toString("hh:mmZ"), fmt);
} //insertTimestampLine

void ChatFormatter::appendMessage(QTextEdit *textEdit, const QString &user, const QString &message, const QDateTime &timestamp, bool isSent)
{
    LOG_DEBUG(Q_FUNC_INFO);

    // QTextCursor cursor = textEdit->textCursor();
    // cursor.movePosition(QTextCursor::End);

    QTextCursor cursor(textEdit->document());
    cursor.movePosition(QTextCursor::End);

    const QColor userColor = resolveUserColor(user, isSent);

    if (!user.isEmpty()) {
        insertBlock(textEdit, cursor, isSent);
        insertUserLine(cursor, user, userColor);
        insertMessageLine(cursor, message);
    } else {
        insertMessageLine(cursor, "\n" + message);
    }

    QString styleSheet = textEdit->styleSheet();

    bool isDark = styleSheet.simplified().contains("QTextEdit#textEditChat { color: black;}");

    insertTimestampLine(cursor, timestamp, textEdit->font(), isDark);

    textEdit->setTextCursor(cursor);
} //appendMessage

QColor ChatFormatter::generateColorForUser(const QString &user)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    // deterministic hash → HSV color
    QByteArray data = user.toUtf8();
    size_t hash = qHash(data);

    int hue = hash % 360;
    int sat = 180 + (hash / 360 % 75);
    int val = 200 + (hash / 10000 % 55);

    return QColor::fromHsv(hue, sat, val);
} //generateColorForUser
