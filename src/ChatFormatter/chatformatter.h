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

#include "../SettingsManager/settingsmanager.h"

class QTextEdit;

#define BORDER_MARGIN 0.15
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
    ///@name Constructor
    ///@{
    /**
     * @brief Constructs a ChatFormatter instance.
     * @param parent The parent QObject.
     */
    explicit ChatFormatter(QObject *parent = nullptr);
    ///@}

    ///@name Public Interface
    ///@{
    /**
     * @brief Appends a formatted chat message to the given QTextEdit.
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
    ///@}

private:
    ///@name Internal State
    ///@{
    QTextCursor lastTimestampCursor;  ///< Cursor for delayed timestamp insertion.
    QDateTime lastTimestamp;          ///< Timestamp of the last message.
    bool hasPendingTimestamp = false; ///< Indicates if a timestamp is pending.
    QMap<QString, QColor> userColorMap; ///< Caches user-specific colors.
    ///@}

    ///@name Message Formatting Helpers
    ///@{
    /**
 * @brief Inserts a paragraph block into the chat with appropriate alignment and margin.
 *
 * @param textEdit The QTextEdit used to calculate dynamic margin width.
 * @param cursor The text cursor at which to insert the block.
 * @param isSent True if the message was sent by the local user; affects alignment.
 */
    void insertBlock(QTextEdit *textEdit, QTextCursor &cursor, bool isSent);

    /**
 * @brief Inserts the sender's name into the chat display with color formatting.
 *
 * Writes the user’s name into the QTextEdit, using a bold and color-styled font.
 * Only shown when the sender changes or for the first message in a series.
 *
 * @param cursor The text cursor to insert at.
 * @param user The name of the user to display.
 * @param color The QColor to use for the user label.
 */
    void insertUserLine(QTextCursor &cursor, const QString &user, const QColor &color);

    /**
 * @brief Inserts the main chat message content.
 *
 * Adds the body of the chat message using the appropriate style.
 * Text inherits the default formatting unless overridden by the stylesheet.
 *
 * @param cursor The text cursor to insert at.
 * @param message The actual message text.
 */
    void insertMessageLine(QTextCursor &cursor, const QString &message);

    /**
 * @brief Inserts a timestamp line below the chat message.
 *
 * Renders a timestamp beneath the chat message in a lighter style and smaller font.
 * Intended to visually separate message clusters and provide time context.
 *
 * @param cursor The text cursor to insert at.
 * @param ts The UTC timestamp of the message.
 * @param baseFont The base font to derive size and style from.
 * @param isDark Whether the current theme is dark (affects timestamp color).
 */
    void insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, const bool &isDark);
    ///@}

    ///@name Color Handling
    ///@{
    /**
 * @brief Resolves the display color for a user, considering message direction.
 *
 * If the user has a cached color in the internal map, it is returned directly.
 * Otherwise, a new color is generated using the user's name. Messages sent by
 * the local user may use a distinct default color.
 *
 * @param user The name of the user whose color is to be resolved.
 * @param isSent True if the message was sent by the local user; affects fallback color.
 * @return The resolved or generated color for the user.
 */
    QColor resolveUserColor(const QString &user, bool isSent);

    /**
 * @brief Generates a deterministic color for a given username.
 *
 * Uses a hash of the username to create a visually distinct and consistent color.
 * Does not cache the result internally — intended for one-time color generation.
 *
 * @param user The user name or identifier.
 * @return A QColor derived from the hashed user name.
 */
    QColor generateColorForUser(const QString &user);

    /**
 * @brief Retrieves or generates and caches a color for the given user.
 *
 * Ensures consistent color use throughout the chat. If the user already has a
 * cached color in the `userColorMap`, that color is returned. Otherwise, a new
 * color is generated using `generateColorForUser()` and stored for reuse.
 *
 * @param user The username or identifier.
 * @return A cached or newly generated QColor.
 */
    QColor generateUserColor(const QString &user);
    ///@}

    ///@name Layout Utility
    ///@{
    /**
 * @brief Calculates a horizontal margin relative to the QTextEdit width.
 *
 * Determines a pixel-based margin based on a percentage of the width of the
 * parent QTextEdit associated with the given cursor. If the parent cannot
 * be determined (e.g., null cursor or missing widget), a fallback value is returned.
 *
 * Commonly used to align message blocks with appropriate indentation depending on layout width.
 *
 * @param cursor The QTextCursor from which the parent QTextEdit is inferred.
 * @param percent A fractional value (e.g., 0.15 for 15%) of the QTextEdit's width.
 * @param fallback The pixel value to return if no valid width can be determined.
 * @return The calculated margin in pixels, or the fallback if width could not be computed.
 */
    int calculateDynamicMargin(QTextEdit *textEdit, double percent, int fallback) const;
    ///@}
};

#endif // CHATFORMATTER_H
