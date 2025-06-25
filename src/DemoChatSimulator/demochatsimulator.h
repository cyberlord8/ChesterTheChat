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

#ifndef DEMOCHATSIMULATOR_H
#define DEMOCHATSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QTextEdit>
#include <QDateTime>

#include <QDir>
#include <QFile>
#include <QFileInfoList>

#include <QTextStream>
#include <QDebug>

#include <QRandomGenerator>

#include <QPointer>

#include "../ChatFormatter/chatformatter.h"

/**
 * @struct DemoMessage
 * @brief Represents a single scripted chat message in the demo sequence.
 *
 * Each DemoMessage holds metadata necessary to render a chat entry in the UI
 * as part of the demo mode. Messages include sender identity, message content,
 * timing, and whether the message is to be treated as if it were sent by the
 * local user (for right-aligned formatting).
 */
struct DemoMessage {
    QString user;         ///< Name of the user (e.g., "Alice").
    QString text;         ///< The actual chat message content.
    int delayMs;          ///< Delay in milliseconds after the previous message.
    bool isSentByMe = false; ///< True if this message should appear as if sent by the local user.
};

/**
 * @class DemoChatSimulator
 * @brief Simulates a themed, time-based chat conversation for demo purposes.
 *
 * The DemoChatSimulator class injects scripted messages from Wonderland-themed
 * characters (e.g., Alice, Mad Hatter) into a chat display with controlled timing.
 * This is used to demonstrate UI features such as message formatting, stacking,
 * and timestamping in a visually dynamic way.
 */
class DemoChatSimulator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs the demo simulator.
     * @param display Pointer to the QTextEdit widget used to render chat messages.
     * @param formatter Pointer to a ChatFormatter used to format each message visually.
     * @param parent Optional parent QObject for ownership and signal propagation.
     */
    DemoChatSimulator(QTextEdit *display, ChatFormatter *formatter, QObject *parent = nullptr);

    /**
     * @brief Starts the demo conversation playback.
     *
     * Initiates a timed sequence of messages, styled and injected into the chat display
     * one at a time to simulate natural conversation flow.
     */
    bool startDemo();

    /**
     * @brief Stops the demo playback.
     *
     * Halts any remaining scheduled messages. Can be used to cancel the demo early.
     */
    void stopDemo();

private:

    bool isRunning = false;

    /**
 * @brief List of full file paths to available demo CSV scripts.
 *
 * Populated from the "demofiles" directory on first use.
 * Each file represents a separate scripted chat conversation
 * that can be rotated through sequentially.
 */
    QStringList demoFiles;




    /**
     * @brief Pointer to the chat display widget where messages are appended.
     */
    // QTextEdit *chatDisplay;
    QPointer<QTextEdit> chatDisplay;

    /**
     * @brief Pointer to the ChatFormatter used to format and color messages.
     */
    ChatFormatter *formatter;

    /**
     * @brief The full list of demo messages to simulate.
     */
    QList<DemoMessage> messageQueue;

    /**
     * @brief Timer used to control the delay between message insertions.
     */
    QTimer messageTimer;

    /**
     * @brief Index of the next message in the queue to be displayed.
     */
    int currentIndex = 0;

    /**
     * @brief Tracks the last sender's name to determine whether to show the username again.
     */
    QString lastUser;

    /**
 * @brief Loads the next demo script from the list of CSV files.
 *
 * This method rotates through all `.csv` files found in the
 * "demofiles" directory. On the first call, it scans the folder
 * and stores a list of available scripts. On subsequent calls,
 * it loads the next one in round-robin fashion using `loadMessagesFromCsv()`.
 *
 * @note If no files are found, the message queue will remain empty.
 */
    bool queueMessages();

    /**
     * @brief Displays the next message in the queue and schedules the next one.
     */
    void showNextMessage();

    /**
 * @brief Loads chat messages from a CSV file into the demo message queue.
 *
 * Parses the provided CSV file to extract a scripted conversation where
 * each line represents a chat message. The file must be in the format:
 *
 * ```
 * user,text,delay_ms
 * Alice,Where am I?,1000
 * Mad Hatter,Have some tea!,2000
 * ```
 *
 * The first row is treated as a header and skipped. Lines beginning with `#`
 * or empty lines are ignored. Parsed messages are stored in `messageQueue`.
 *
 * @param filePath Absolute or relative path to the CSV file containing demo messages.
 *
 * @warning If the file cannot be opened or is malformed, the method will log
 * a warning and `messageQueue` may remain empty or partially filled.
 *
 * @see queueMessages(), loadDemoFileList()
 */
    void loadMessagesFromCsv(const QString &filePath);

    /**
 * @brief Scans the specified directory for `.csv` demo files.
 *
 * Populates `demoFiles` with the absolute paths of all `.csv` files
 * in the given directory. Called automatically by `queueMessages()`
 * during the first run.
 *
 * @param directory Absolute path to the demo file folder (e.g., "demofiles/").
 */
    void loadDemoFileList(const QString &directory);

    /**
 * @brief Estimates a suitable delay (in milliseconds) for displaying a chat message.
 *
 * This method dynamically calculates the time a user might take to read a given message.
 * It considers the number of characters and adds an optional pause for dramatic punctuation
 * (e.g., '.', '!', '?') to simulate a more natural conversational rhythm.
 *
 * @param text The message content to analyze.
 * @return Estimated delay in milliseconds before the next message should appear.
 *
 * @note The formula assumes a base read speed of ~15 characters per second,
 *       with an additional 100–300 ms depending on punctuation.
 */
    int calculateDynamicDelay(const QString &text) const;

signals:
    void signalRequestClearChatDisplay();
};

#endif // DEMOCHATSIMULATOR_H
