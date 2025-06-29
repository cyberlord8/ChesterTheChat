#include "DemoChatSimulator.h"
#include "../Utils/debugmacros.h"

#include <QCoreApplication>
#include <QRegularExpression>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTextEdit>
#include <QRandomGenerator>


DemoChatSimulator::DemoChatSimulator(QTextEdit *display, ChatFormatter *formatter, QObject *parent)
    : QObject(parent)
    , chatDisplay(display)
    , formatter(formatter)
{
    LOG_DEBUG(Q_FUNC_INFO);

    connect(&messageTimer, &QTimer::timeout, this, &DemoChatSimulator::showNextMessage);
}//DemoChatSimulator

bool DemoChatSimulator::startDemo()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!queueMessages())
        return false;

    isRunning = true;

    currentIndex = 0;
    lastUser.clear();
    messageTimer.start(500);
    return true;
}//startDemo

void DemoChatSimulator::stopDemo()
{
    LOG_DEBUG(Q_FUNC_INFO);

    messageTimer.stop();
    disconnect();  // Disconnects all slots from this object

    currentIndex = 0;
    messageQueue.clear();
    lastUser.clear();

    isRunning = false;
}//stopDemo

void DemoChatSimulator::loadDemoFileList(const QString &directory)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QDir dir(directory);
    if (!dir.exists()) {
        qWarning() << "[DemoChatSimulator] Directory not found:" << directory;
        return;
    }

    const QStringList filters = QStringList() << "*.csv";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::Readable);

    for (const QFileInfo &file : files) {
        demoFiles.append(file.absoluteFilePath());
    }

    std::sort(demoFiles.begin(), demoFiles.end()); // Optional: alphabetical order
}//loadDemoFileList

bool DemoChatSimulator::queueMessages()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString demoDir = QCoreApplication::applicationDirPath() + "/demofiles";

    demoFiles.clear();
    loadDemoFileList(demoDir);
    if (demoFiles.isEmpty()) {
        qWarning() << "[DemoChatSimulator] No demo CSV files found in" << demoDir;
        return false;
    }

    messageQueue.clear();
    for (int demoFileIndex = 0; demoFileIndex < demoFiles.size(); ++demoFileIndex) {
        const QString filePath = demoFiles[demoFileIndex];

        loadMessagesFromCsv(filePath);
    }

    if (messageQueue.isEmpty()) {
        qWarning() << "[DemoChatSimulator] CSV file loaded but contained no valid messages.";
        return false;
    }

    return true;
} //queueMessages

void DemoChatSimulator::showNextMessage()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (currentIndex >= messageQueue.size()) {
        LOG_DEBUG(Q_FUNC_INFO);
        messageTimer.stop();


        QPointer<DemoChatSimulator> self(this);  // weak pointer to self

        QTimer::singleShot(2000, this, [self]() {
            if (!self || !self->isRunning)
                return;
            self->startDemo();
        });

        return;
    }//if we're out of messages, restart demo

    const DemoMessage &msg = messageQueue[currentIndex++];

    if (msg.user == "System") {
        QTextCursor cursor(chatDisplay->document());
        cursor.movePosition(QTextCursor::End);

        QTextBlockFormat blockFmt;
        blockFmt.setAlignment(Qt::AlignHCenter); // force center alignment
        cursor.insertBlock(blockFmt);

        QTextCharFormat charFmt;
        charFmt.setForeground(Qt::gray);
        charFmt.setFontItalic(true);
        charFmt.setFontPointSize(11);

        cursor.insertText("\n" + msg.text, charFmt);

        if(!chatDisplay)
            return;
        chatDisplay->setTextCursor(cursor);
        messageTimer.start(msg.delayMs);
        return;
    }


    // Regular message logic
    bool showUser = (msg.user != lastUser);
    lastUser = msg.user;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    if(!chatDisplay)
        return;
    formatter->appendMessage(chatDisplay,
                             showUser ? msg.user : "",
                             msg.text,
                             now,
                             msg.isSentByMe);

    messageTimer.start(msg.delayMs);
} //showNextMessage

int DemoChatSimulator::calculateDynamicDelay(const QString &text) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    const int baseDelay = 500;
    const double readSpeedCharsPerSec = 5.0;

    int length = text.length();
    int readTimeMs = static_cast<int>((length / readSpeedCharsPerSec) * 1000);

    int dramaticPause = text.contains(QRegularExpression("[.!?]")) ? 300 : 100;

    return baseDelay + readTimeMs + dramaticPause;
} //calculateDynamicDelay

void DemoChatSimulator::loadMessagesFromCsv(const QString &filePath) {
    LOG_DEBUG(Q_FUNC_INFO);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[DemoChatSimulator] Failed to open CSV:" << filePath;
        return;
    }

    QTextStream in(&file);
    QStringList tempUsers;
    QList<DemoMessage> messagesFromFile;

    bool isFirstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        const QStringList parts = line.split(',', Qt::KeepEmptyParts);
        if (parts.size() < 2) continue;

        QString user = parts[0].trimmed();
        QString text = parts[1].trimmed();

        if (!tempUsers.contains(user))
            tempUsers.append(user);

        int delay = calculateDynamicDelay(text);

        messagesFromFile.append({ user, text, delay, false }); // we'll update `isSentByMe` below
    }

    // Select a random "local user" for this chat file
    QString selectedLocalUser;
    if (!tempUsers.isEmpty()) {
        int index = QRandomGenerator::global()->bounded(tempUsers.size());
        selectedLocalUser = tempUsers[index];
    }

    QString fileTitle = QFileInfo(filePath).baseName(); // e.g., "mad_tea_party"
    QString introText = QString("You are now playing as %1 — (%2)")
                            .arg(selectedLocalUser, fileTitle);

    messageQueue.append({
        "System",        // Special marker
        introText,       // HTML-styled text
        1000,            // Short pause
        false            // Do not align as "sent by me"
    });

    for (DemoMessage &msg : messagesFromFile) {
        msg.isSentByMe = (msg.user == selectedLocalUser);
        messageQueue.append(msg);
    }

    file.close();
}//loadMessagesFromCsv
