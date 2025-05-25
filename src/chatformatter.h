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
#include <QtGlobal>
#include <QTimer>
#include <QTextBlock>

#include "settingsmanager.h"

class QTextEdit;

/**
 * @class ChatFormatter
 * @brief Responsible for formatting and inserting chat messages into a QTextEdit.
 *
 * ChatFormatter handles text alignment, coloring, and formatting of messages
 * based on whether they are sent or received. It also assigns and caches user-specific
 * colors and adjusts styling based on the theme (dark/light).
 */
class ChatFormatter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a ChatFormatter instance.
     * @param parent The parent QObject.
     */
    explicit ChatFormatter(QObject *parent = nullptr);

    /**
     * @brief Appends a formatted chat message to the given QTextEdit.
     *
     * @param textEdit Pointer to the QTextEdit where the message will be appended.
     * @param user The name of the user who sent the message.
     * @param message The content of the message.
     * @param timestamp The time the message was sent.
     * @param isSent Whether the message was sent by the local user.
     */
    void appendMessage(QTextEdit *textEdit,
                       const QString &user,
                       const QString &message,
                       const QDateTime &timestamp,
                       bool isSent);

private:
    /**
 * @brief Stores the cursor position for the last message that may need a delayed timestamp.
 */
    QTextCursor lastTimestampCursor;

    /**
 * @brief Timestamp of the last message, used for delayed timestamp insertion.
 */
    QDateTime lastTimestamp;

    /**
 * @brief Flag indicating whether a timestamp is pending for insertion.
 */
    bool hasPendingTimestamp = false;

    /**
     * @brief Generates a consistent color for a given user.
     * @param user The user identifier.
     * @return A QColor object deterministically based on the user's name.
     */
    QColor generateColorForUser(const QString &user);

    /**
     * @brief Inserts a new text block with appropriate alignment into the text cursor.
     * @param cursor The text cursor to modify.
     * @param isSent Whether the message was sent by the local user.
     */
    void insertBlock(QTextCursor &cursor, bool isSent);

    /**
     * @brief Resolves or generates the display color for a given user.
     * @param user The user identifier.
     * @param isSent Whether the message was sent by the local user.
     * @return The QColor associated with the user.
     */
    QColor resolveUserColor(const QString &user, bool isSent);

    /**
     * @brief Inserts the user name line with specific formatting.
     * @param cursor The text cursor to insert into.
     * @param user The user name.
     * @param color The color to use for the text.
     */
    void insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color);

    /**
     * @brief Inserts the main message line with formatting.
     * @param cursor The text cursor to insert into.
     * @param message The message content.
     * @param isDark Whether the theme is dark.
     */
    void insertMessageLine(QTextCursor &cursor, const QString &message);

    /**
     * @brief Inserts the timestamp line beneath the message.
     * @param cursor The text cursor to insert into.
     * @param ts The message timestamp.
     * @param baseFont The base font to inherit style from.
     * @param isDark Whether the theme is dark.
     */
    void insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, const bool &isDark);

    /**
     * @brief Stores user-to-color mappings for consistent chat display.
     */
    QMap<QString, QColor> userColorMap;

    /**
 * @brief Calculates a dynamic margin based on the width of the parent QTextEdit.
 *
 * This utility determines a margin value as a percentage of the visible width of the QTextEdit
 * that owns the given QTextCursor. If the QTextEdit cannot be determined, a fallback value is used.
 *
 * @param cursor A QTextCursor from which the parent QTextEdit is inferred.
 * @param percent The margin as a fraction of the QTextEdit's width (e.g., 0.15 for 15%).
 * @param fallback The pixel value to use if the QTextEdit or its viewport cannot be resolved.
 * @return The calculated margin in pixels.
 */
    int calculateDynamicMargin(QTextCursor &cursor, double percent, int fallback) const;

    /**
 * @brief Retrieves the color assigned to a specific user, generating and caching a new color if needed.
 *
 * This function ensures consistent user-specific coloring in the chat display. If the user has not
 * yet been assigned a color, one is generated deterministically using a hash-based method and stored
 * in the internal cache for future reuse.
 *
 * @param user The identifier (typically a username) whose color should be retrieved.
 * @return A QColor uniquely associated with the specified user.
 */
    QColor generateUserColor(const QString &user);
};

#endif // CHATFORMATTER_H
