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

#include "chatsessionlogger.h"

#include <QByteArray>
#include <QDir>
#include <QIODevice>

namespace {

constexpr int TimestampColumnWidth = 24;
constexpr int DirectionColumnWidth = 4;
constexpr int UserColumnWidth = 12;

QString formatUtcTimestamp(const QDateTime &timestamp)
{
    return timestamp.toUTC().toString("yyyy-MM-dd'T'HH:mm:ss.zzz'Z'");
}

} // namespace

ChatSessionLogger::ChatSessionLogger() = default;

ChatSessionLogger::~ChatSessionLogger()
{
    endSession();
} //ChatSessionLogger

bool ChatSessionLogger::startSession(const QString &baseDirectory,
                                     int instanceId,
                                     const QString &localUser,
                                     const QString &localAddress,
                                     quint16 localPort,
                                     const QString &remoteAddress,
                                     quint16 remotePort)
{
    if (isActive())
        endSession();

    m_lastError.clear();

    const QDateTime startTime = QDateTime::currentDateTimeUtc();
    const QString logDirectoryPath = QDir(baseDirectory).filePath("logs");

    if (!QDir().mkpath(logDirectoryPath)) {
        m_lastError = QString("Unable to create log directory: %1").arg(logDirectoryPath);
        return false;
    }

    const QString fileTimestamp = startTime.toString("yyyyMMdd_HHmmss_zzz'Z'");

    const QString baseFileName
        = QString("ChesterChat_%1_instance_%2").arg(fileTimestamp).arg(instanceId);

    QString fileName = baseFileName + ".txt";
    int suffix = 2;

    while (QFile::exists(QDir(logDirectoryPath).filePath(fileName))) {
        fileName = QString("%1_%2.txt").arg(baseFileName).arg(suffix++);
    }

    m_logFilePath = QDir(logDirectoryPath).filePath(fileName);
    m_logFile.setFileName(m_logFilePath);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Unable to open session log %1: %2")
                          .arg(m_logFilePath, m_logFile.errorString());
        return false;
    }

    const bool headerWritten
        = writeLine("Chester The Chat Session Log")
          && writeLine(QString("Started (UTC): %1").arg(formatUtcTimestamp(startTime)))
          && writeLine(QString("Instance: %1").arg(instanceId))
          && writeLine(QString("Local user: %1").arg(escapeField(localUser)))
          && writeLine(
              QString("Local endpoint: %1:%2").arg(escapeField(localAddress)).arg(localPort))
          && writeLine(
              QString("Remote endpoint: %1:%2").arg(escapeField(remoteAddress)).arg(remotePort))
          && writeLine(QString())
          && writeLine(QString("%1 | %2 | %3 | %4")
                           .arg(QStringLiteral("Timestamp (UTC)"), -TimestampColumnWidth)
                           .arg(QStringLiteral("DIR"), -DirectionColumnWidth)
                           .arg(QStringLiteral("User"), -UserColumnWidth)
                           .arg(QStringLiteral("Message")));

    if (!headerWritten) {
        m_logFile.close();
        return false;
    }

    return true;
} //startSession

bool ChatSessionLogger::logMessage(Direction direction,
                                   const QString &user,
                                   const QString &message,
                                   const QDateTime &timestamp)
{
    const QString directionText = direction == Direction::Sent ? "SENT" : "RCVD";

    const QString entry = QString("%1 | %2 | %3 | %4")
                              .arg(formatUtcTimestamp(timestamp), -TimestampColumnWidth)
                              .arg(directionText, -DirectionColumnWidth)
                              .arg(escapeField(user), -UserColumnWidth)
                              .arg(escapeField(message));

    return writeLine(entry);
} //logMessage

void ChatSessionLogger::endSession()
{
    if (!isActive())
        return;

    writeLine(QString());
    writeLine(QString("Ended (UTC): %1").arg(formatUtcTimestamp(QDateTime::currentDateTimeUtc())));

    m_logFile.close();
} //endSession

bool ChatSessionLogger::isActive() const
{
    return m_logFile.isOpen();
} //isActive

QString ChatSessionLogger::filePath() const
{
    return m_logFilePath;
} //filePath

QString ChatSessionLogger::lastError() const
{
    return m_lastError;
} //lastError

QString ChatSessionLogger::escapeField(const QString &text)
{
    QString escaped = text;

    escaped.replace("\\", "\\\\");
    escaped.replace("\r", "\\r");
    escaped.replace("\n", "\\n");
    escaped.replace("\t", "\\t");
    escaped.replace("|", "\\|");

    return escaped;
} //escapeField

bool ChatSessionLogger::writeLine(const QString &line)
{
    if (!m_logFile.isOpen()) {
        m_lastError = "No active chat-session log.";
        return false;
    }

    QByteArray data = line.toUtf8();
    data.append('\n');

    if (m_logFile.write(data) != data.size()) {
        m_lastError = QString("Unable to write session log: %1").arg(m_logFile.errorString());
        return false;
    }

    if (!m_logFile.flush()) {
        m_lastError = QString("Unable to flush session log: %1").arg(m_logFile.errorString());
        return false;
    }

    return true;
} //writeLine