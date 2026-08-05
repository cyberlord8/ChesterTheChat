/*
 * Chester The Chat
 * Copyright (C) 2024 - 2026 Timothy Millea
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

#ifndef CHATSESSIONLOGGER_H
#define CHATSESSIONLOGGER_H

#include <QDateTime>
#include <QFile>
#include <QString>
#include <QtGlobal>

/**
 * @class ChatSessionLogger
 * @brief Writes sent and received chat messages to a UTF-8 session log.
 *
 * Each successful Connect-to-Disconnect session produces a separate
 * human-readable text file.
 */
class ChatSessionLogger
{
public:

    /**
     * @brief Deletes expired Chester session logs.
     * @param baseDirectory Directory containing Chester's logs subdirectory.
     * @param retentionDays Maximum log age in days; zero keeps logs forever.
     * @return true if cleanup completed without errors.
     */
    bool purgeOldLogs(const QString &baseDirectory, int retentionDays);

    /**
     * @brief Identifies the local perspective of a logged message.
     */
    enum class Direction {
        Sent,
        Received
    };

    ChatSessionLogger();
    ~ChatSessionLogger();

    /**
     * @brief Creates and opens a new chat-session log.
     *
     * @param baseDirectory Chester's application directory.
     * @param instanceId The running Chester instance number.
     * @param localUser The local username.
     * @param localAddress The local UDP address.
     * @param localPort The local UDP port.
     * @param remoteAddress The remote or multicast UDP address.
     * @param remotePort The remote UDP port.
     * @return True if the session log was opened successfully.
     */
    bool startSession(const QString &baseDirectory,
                      int instanceId,
                      const QString &localUser,
                      const QString &localAddress,
                      quint16 localPort,
                      const QString &remoteAddress,
                      quint16 remotePort);

    /**
     * @brief Writes one message to the active session log.
     *
     * @param direction Whether the message was sent or received.
     * @param user The message sender's username.
     * @param message The message text.
     * @param timestamp The message's UTC timestamp.
     * @return True if the entry was written successfully.
     */
    bool logMessage(Direction direction,
                    const QString &user,
                    const QString &message,
                    const QDateTime &timestamp);

    /**
     * @brief Writes the session footer and closes the log file.
     */
    void endSession();

    /**
     * @brief Returns whether a session log is currently open.
     */
    bool isActive() const;

    /**
     * @brief Returns the active or most recently used log-file path.
     */
    QString filePath() const;

    /**
     * @brief Returns the most recent file error.
     */
    QString lastError() const;

private:

    /**
     * @brief Escapes characters that would disrupt the one-entry-per-line format.
     */
    static QString escapeField(const QString &text);

    /**
     * @brief Writes and immediately flushes one UTF-8 line.
     */
    bool writeLine(const QString &line);

    QFile m_logFile;
    QString m_logFilePath;
    QString m_lastError;
};

#endif // CHATSESSIONLOGGER_H