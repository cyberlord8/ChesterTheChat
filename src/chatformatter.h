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
     * @param isDarkThemed Whether the UI is using a dark theme.
     * @param isSent Whether the message was sent by the local user.
     */
    void appendMessage(QTextEdit *textEdit,
                       const QString &user,
                       const QString &message,
                       const QDateTime &timestamp,
                       bool isDarkThemed,
                       bool isSent);

private:
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
    void insertMessageLine(QTextCursor &cursor, const QString &message, bool isDark);

    /**
     * @brief Inserts the timestamp line beneath the message.
     * @param cursor The text cursor to insert into.
     * @param ts The message timestamp.
     * @param baseFont The base font to inherit style from.
     * @param isDark Whether the theme is dark.
     */
    void insertTimestampLine(QTextCursor &cursor, const QDateTime &ts, const QFont &baseFont, bool isDark);

    /**
     * @brief Stores user-to-color mappings for consistent chat display.
     */
    QMap<QString, QColor> userColorMap;
};


#endif // CHATFORMATTER_H
