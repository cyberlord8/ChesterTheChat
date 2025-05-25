#include "DemoChatSimulator.h"

DemoChatSimulator::DemoChatSimulator(QTextEdit *display, ChatFormatter *formatter, QObject *parent)
    : QObject(parent)
    , chatDisplay(display)
    , formatter(formatter)
{
    connect(&messageTimer, &QTimer::timeout, this, &DemoChatSimulator::showNextMessage);
}//DemoChatSimulator

bool DemoChatSimulator::startDemo()
{
    if (!queueMessages())
        return false;

    currentIndex = 0;
    lastUser.clear();
    messageTimer.start(500);
    chatDisplay->clear();
    return true;
}//startDemo

void DemoChatSimulator::stopDemo()
{
    messageTimer.stop();
}//stopDemo

void DemoChatSimulator::loadDemoFileList(const QString &directory)
{
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
        qDebug() << "[DemoChatSimulator] Loading demo file:" << filePath;

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
    if (currentIndex >= messageQueue.size()) {
        messageTimer.stop();

        // Optional: add a pause before restarting
        QTimer::singleShot(2000, this, [this]() {
            startDemo();
        });

        return;
    }


    const DemoMessage &msg = messageQueue[currentIndex++];

    if (msg.user == "System") {
        QTextCursor cursor = chatDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);

        QTextBlockFormat blockFmt;
        blockFmt.setAlignment(Qt::AlignHCenter); // force center alignment
        cursor.insertBlock(blockFmt);

        QTextCharFormat charFmt;
        charFmt.setForeground(Qt::gray);
        charFmt.setFontItalic(true);
        charFmt.setFontPointSize(11);

        cursor.insertText("\n" + msg.text, charFmt);

        chatDisplay->setTextCursor(cursor);
        messageTimer.start(msg.delayMs);
        return;
    }


    // Regular message logic
    bool showUser = (msg.user != lastUser);
    lastUser = msg.user;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    formatter->appendMessage(chatDisplay,
                             showUser ? msg.user : "",
                             msg.text,
                             now,
                             // isDarkTheme,
                             msg.isSentByMe);

    messageTimer.start(msg.delayMs);
} //showNextMessage

int DemoChatSimulator::calculateDynamicDelay(const QString &text) const
{
    const int baseDelay = 500;
    const double readSpeedCharsPerSec = 5.0;

    int length = text.length();
    int readTimeMs = static_cast<int>((length / readSpeedCharsPerSec) * 1000);

    int dramaticPause = text.contains(QRegularExpression("[.!?]")) ? 300 : 100;

    return baseDelay + readTimeMs + dramaticPause;
} //calculateDynamicDelay

void DemoChatSimulator::loadMessagesFromCsv(const QString &filePath) {
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
        qDebug() << "[DemoChatSimulator] Selected local user for file:" << QFileInfo(filePath).fileName() << "→" << selectedLocalUser;
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
