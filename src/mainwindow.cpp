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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "globals.h"

#include "toastnotification.h"

#include "src/debugmacros.h"

#include <QFile>
#include <QLockFile>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QNetworkInterface>
#include <QWheelEvent>
#include <QDirIterator>
#include <QScrollBar>
#include <QTextEdit>

QString MainWindow::buildVersionSuffix(const QString &version) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (version.contains("alpha", Qt::CaseInsensitive)) {
        QString buildDate = tr(BUILDDATE);
        int trimIndex = buildDate.lastIndexOf(':');
        if (trimIndex != -1)
            buildDate = buildDate.left(trimIndex);

        return QString(" - %1 - %2").arg(version, buildDate);
    }

    if (version.contains("beta", Qt::CaseInsensitive)) {
        return QString(" - %1 - %2").arg(version, tr(RELEASEDATE));
    }

    return QString(" - %1").arg(tr(RELEASEDATE));
} //buildVersionSuffix

void MainWindow::setAppWindowTitle()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString userName = configSettings.userName;
    const QString baseTitle = tr(APP_NAME) + QString(" - %1_%2").arg(QString::number(instanceID), userName);

    const QString versionSuffix = buildVersionSuffix(tr(VERSION));

    setWindowTitle(baseTitle + versionSuffix);
} //setAppWindowTitle

QString MainWindow::getInstanceIdsFilePath() const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    return QCoreApplication::applicationDirPath() + "/instance_ids.txt";
} //getInstanceIdsFilePath

QString MainWindow::getInstanceIdsLockFilePath() const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    return QCoreApplication::applicationDirPath() + "/instance_ids.lock";
} //getInstanceIdsLockFilePath

QSet<int> MainWindow::loadInstanceIdsExcluding(int excludeId, const QString &filePath) const
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QSet<int> ids;
    QFile file(filePath);

    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ids;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        const int id = line.toInt();
        if (excludeId < 0 || id != excludeId)
            ids.insert(id);
    }

    file.close();
    return ids;
} //loadInstanceIdsExcluding

void MainWindow::saveInstanceIds(const QSet<int> &ids, const QString &filePath) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream out(&file);
    for (int id : std::as_const(ids)) {
        out << id << '\n';
    }

    file.close();
} //saveInstanceIds

void MainWindow::releaseInstanceId(int instanceId)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (instanceId < 1)
        return;

    const QString idsFilePath = getInstanceIdsFilePath();
    const QString lockFilePath = getInstanceIdsLockFilePath();

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("[MainWindow] Failed to lock %s for releasing instance ID %d", qUtf8Printable(lockFilePath), instanceId);
        return;
    }

    QSet<int> remainingIds = loadInstanceIdsExcluding(instanceId, idsFilePath);
    saveInstanceIds(remainingIds, idsFilePath);

    lock.unlock();
} //releaseInstanceId

int MainWindow::acquireInstanceId()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString idsFilePath = getInstanceIdsFilePath();
    const QString lockFilePath = getInstanceIdsLockFilePath();

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("[MainWindow] Failed to lock %s for acquiring instance ID", qUtf8Printable(lockFilePath));
        return -1;
    }

    QSet<int> usedIds = loadInstanceIdsExcluding(-1, idsFilePath); // Load all IDs

    int newId = 1;
    while (usedIds.contains(newId)) {
        ++newId;
    }

    usedIds.insert(newId);
    saveInstanceIds(usedIds, idsFilePath);

    lock.unlock();
    return newId;
} //acquireInstanceId

QString MainWindow::buildAppVersionString() const
{
    LOG_DEBUG(Q_FUNC_INFO);

    QString version = VERSION;

    if (version.contains("alpha", Qt::CaseInsensitive) || version.contains("bravo", Qt::CaseInsensitive)) {
        version += " " + tr(__TIME__); // Append compile time for dev builds
    }

    return version;
} //buildAppVersionString

QString MainWindow::detectCompilerInfo() const
{
    LOG_DEBUG(Q_FUNC_INFO);

#ifdef ENV32
    return QString("Qt %1 - MinGW (32-bit)").arg(qVersion());
#else
    return QString("Qt %1 - MinGW (64-bit)").arg(qVersion());
#endif
} //detectCompilerInfo

QString MainWindow::buildAboutText(const QString &version, const QString &compileDate, const QString &releaseDate, const QString &compilerInfo) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    QString about;
    const QString separator = tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    about += tr(APP_LICENSE) + '\n';
    about += separator;
    about += QString(" - %1 %2 %3\n").arg(COPYRIGHT, compileDate, COMPANY);
    about += QString(" - %1\n").arg(RIGHTS);
    about += separator;
    about += QString(" - %1%2\n").arg(tr("Version: "), version);
    about += QString(" - %1%2\n").arg(tr("Release date: "), releaseDate);
    about += QString(" - %1%2\n").arg(tr("Compiler: "), compilerInfo);
    about += QString(" - %1%2\n").arg(tr("Running on: "), SYSTEMINFO);
    about += separator;
    about += QString("%1\n").arg(WARRANTY);
    about += separator;
    about += tr("This product uses Qt Libraries:\n");
    about += QString(" - %1\n").arg(qt_COPYRIGHT);
    about += QString(" - %1%2\n").arg(tr("QT Library License: "), qt_LICENSE);
    about += separator;

    return about;
} //buildAboutText

void MainWindow::on_actionAbout_triggered()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString version = buildAppVersionString();
    const QString compileDate = QStringLiteral(__DATE__); // e.g. "May 23 2025"
    const QString releaseDate = QLocale().toString(QDate::fromString(compileDate, "MMM dd yyyy"), QLocale::ShortFormat);

    const QString compilerInfo = detectCompilerInfo();
    const QString aboutText = buildAboutText(version, compileDate, releaseDate, compilerInfo);

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr(APP_NAME));
    aboutBox.setText(aboutText);
    aboutBox.exec();
} //on_actionAbout_triggered

void MainWindow::on_actionAbout_Qt_triggered()
{
    LOG_DEBUG(Q_FUNC_INFO);

    qApp->aboutQt();
} //on_actionAbout_Qt_triggered

bool MainWindow::copyResourceToFile(const QString &resourcePath, const QString &targetPath)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QFile resourceFile(resourcePath);
    QFile targetFile(targetPath);

    if (targetFile.exists()) {
        // Try to set permissive file mode before removal (especially for read-only files)
        if (!targetFile.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::WriteUser)) {
            qWarning().nospace() << "[copyResourceToFile] Cannot change permissions on: " << targetPath << " | Reason: " << targetFile.errorString();
        }

        if (!targetFile.remove()) {
            qCritical().nospace() << "[copyResourceToFile] Failed to remove: " << targetPath << " | Reason: " << targetFile.errorString();
            return false;
        }

        qDebug().nospace() << "[copyResourceToFile] Existing file removed: " << targetPath;
    } else {
        qDebug().nospace() << "[copyResourceToFile] No existing file to replace at: " << targetPath;
    }

    if (!resourceFile.copy(targetPath)) {
        qCritical().nospace() << "[copyResourceToFile] Copy failed: " << resourcePath << " → " << targetPath << " | Reason: " << resourceFile.errorString();
        return false;
    }

    qDebug().nospace() << "[copyResourceToFile] Resource copied successfully to: " << targetPath;
    return true;
} //copyResourceToFile

void MainWindow::openResourceFile(const QString &fileName)
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString resourcePath = QStringLiteral(":/resources") + fileName;
    const QString targetPath = qApp->applicationDirPath() + QDir::separator() + fileName;

    if (!copyResourceToFile(resourcePath, targetPath)) {
        qWarning().nospace() << "[openResourceFile] Failed to open resource file: " << targetPath;
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath))) {
        qWarning().nospace() << "[openResourceFile] Failed to launch file: " << targetPath;
    }
} //openResourceFile

void MainWindow::on_actionQt_License_triggered()
{
    LOG_DEBUG(Q_FUNC_INFO);

    openResourceFile("/LGPLv3.pdf");
} //on_actionQt_License_triggered

void MainWindow::on_actionUser_Manual_triggered()
{
    LOG_DEBUG(Q_FUNC_INFO);

    openResourceFile("/UserManual.pdf");
} //on_actionUser_Manual_triggered

bool MainWindow::isValidIPv4Address(const QString &ip) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    return !ip.contains(":") && !ip.startsWith("169.254");
} //isValidIPv4Address

void MainWindow::fillNetworkWidgets()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->comboBoxLocalUDPNetwork->clear();
    ui->comboBoxLocalUDPNetwork->addItem("ANY");

    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : addresses) {
        const QString ip = address.toString();

        if (isValidIPv4Address(ip)) {
            ui->comboBoxLocalUDPNetwork->addItem(ip);
        }
    }
} //fillNetworkWidgets

void MainWindow::updateUIWidgets()
{
    LOG_DEBUG(Q_FUNC_INFO);

    // Network Settings
    ui->lineEditUserName->setText(configSettings.userName);
    ui->checkBoxLoopback->setChecked(configSettings.b_loopback);
    ui->checkBoxMulticast->setChecked(configSettings.b_multicast);
    ui->spinBoxTTL->setValue(configSettings.udpTTL);
    ui->comboBoxLocalUDPNetwork->setCurrentText(configSettings.localUDPAddress);
    ui->lineEditLocalUDPPort->setText(QString::number(configSettings.localUDPPort));
    ui->lineEditRemoteUDPNetwork->setText(configSettings.remoteUDPAddress);
    ui->lineEditRemoteUDPPort->setText(QString::number(configSettings.remoteUDPPort));

    // Visual Options
    ui->checkBoxDisplayBackgroundImage->setChecked(configSettings.b_displayBackgroundImage);

    // Style Sheet Settings
    ui->checkBoxLoadStyleSheet->setChecked(configSettings.b_loadStyleSheet);
    QSignalBlocker block(ui->comboBoxSelectStyleSheet);
    ui->comboBoxSelectStyleSheet->setCurrentText(configSettings.stylesheetName);
} //updateUIWidgets

void MainWindow::setStyleSheet()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!configSettings.b_loadStyleSheet) {
        qDebug() << "[setStyleSheet] Skipping stylesheet load: setting disabled.";
        return;
    }

    const QString displayName = QStyleSheetMap.key(configSettings.stylesheetName);

    if (!displayName.isEmpty()) {
        QSignalBlocker block(ui->comboBoxSelectStyleSheet);
        ui->comboBoxSelectStyleSheet->setCurrentText(displayName);
        loadStyleSheet();
    } else {
        qWarning() << "[setStyleSheet] Style sheet not found in map for file:" << configSettings.stylesheetName;
    }
} //setStyleSheet

QString MainWindow::chatBackgroundStyle() const
{
    LOG_DEBUG(Q_FUNC_INFO);

    return QStringLiteral("QTextEdit#textEditChat { "
                          "border-image: url(:/images/BackgroundImage10.png); "
                          "}");
} //chatBackgroundStyle

void MainWindow::setBackgroundImage()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (configSettings.b_displayBackgroundImage) {
        ui->textEditChat->setStyleSheet(chatBackgroundStyle());
    } else {
        ui->textEditChat->setStyleSheet(QString()); // reset to default
    }
} //setBackgroundImage

void MainWindow::displayMessages(const QList<Message> &messages)
{
    LOG_DEBUG(Q_FUNC_INFO);

    visibleOffset = currentOffset;
    visibleLimit = messages.count();

    ui->textEditChat->clear();

    QString previousUser;
    for (const Message &msg : messages) {
        bool showUserName = (msg.user != previousUser);
        previousUser = msg.user;

        m_formatter->appendMessage(ui->textEditChat, showUserName ? msg.user : "", msg.text, msg.timestamp, msg.isSentByMe);
    }
} //displayMessages

int MainWindow::calculateClampedOffset(int requestedOffset, int totalMessages) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    int maxOffset = qMax(0, totalMessages - messagesPerPage);
    return qBound(0, requestedOffset, maxOffset);
} //calculateClampedOffset

void MainWindow::loadPage(int offset)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isLoadingHistory || offset == currentOffset)
        return;

    isLoadingHistory = true;

    const int total = messageStore->messageCount();
    if (total == 0) {
        isLoadingHistory = false;
        return;
    }

    const int clampedOffset = calculateClampedOffset(offset, total);
    if (clampedOffset == currentOffset) {
        isLoadingHistory = false;
        return;
    }

    const QList<Message> messages = messageStore->fetchMessages(clampedOffset, messagesPerPage);
    displayMessages(messages);

    currentOffset = clampedOffset;
    isLoadingHistory = false;
} //loadPage

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    if (obj == ui->textEditChat->verticalScrollBar() && event->type() == QEvent::Type::Wheel && !isLoadingHistory) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        const int delta = wheelEvent->angleDelta().y();
        handleChatScroll(ui->textEditChat->verticalScrollBar(), delta < 0); // true if scrolling down
    }
    return QMainWindow::eventFilter(obj, event);
} //eventFilter

void MainWindow::handleChatScroll(QScrollBar *scrollBar, bool scrollingDown)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    if (isLoadingHistory) {
        return;
    }

    const int value = scrollBar->value();
    const int min = scrollBar->minimum();
    const int max = scrollBar->maximum();
    const int total = messageStore->messageCount();

    if (scrollingDown && value == max) {
        if ((currentOffset + messagesPerPage) < total) {
            loadPage(currentOffset + messagesPerPage);

            QTimer::singleShot(0, [this]() {
                QScrollBar *sb = ui->textEditChat->verticalScrollBar();
                if ((currentOffset + messagesPerPage) < messageStore->messageCount()) {
                    sb->setValue(sb->minimum() + 1);
                }
            });
        }
    }

    if (!scrollingDown && value == min) {
        if (currentOffset > 0) {
            loadPage(currentOffset - messagesPerPage);

            QTimer::singleShot(0, [this]() {
                QScrollBar *sb = ui->textEditChat->verticalScrollBar();
                if (currentOffset == 0 || messageStore->messageCount() < messagesPerPage) {
                    sb->setValue(sb->minimum());
                }
            });
        }
    }
} //handleChatScroll

void MainWindow::onChatScrollBarChanged(int value)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    if(isDemoRunning)
        return;

    if (lastScrollBarValue < 0) {
        lastScrollBarValue = value;
        return;
    }

    const bool scrollingDown = (value > lastScrollBarValue);
    lastScrollBarValue = value;

    handleChatScroll(ui->textEditChat->verticalScrollBar(), scrollingDown);
} //onChatScrollBarChanged

void MainWindow::startDemoMode()
{
    LOG_DEBUG(Q_FUNC_INFO);

    demoSimulator->startDemo();
    styleRotator->start();
}

void MainWindow::initializeManagers()
{
    LOG_DEBUG(Q_FUNC_INFO);

    settingsManager = new SettingsManager(instanceID, QCoreApplication::applicationDirPath(), this);
    udpManager = new UdpChatSocketManager(this);
    m_formatter = new ChatFormatter(this);

    demoSimulator = new DemoChatSimulator(ui->textEditChat, m_formatter, this);
} //initializeManagers

void MainWindow::initializeUi()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setTabEnabled(0, false); // Disable Chat tab
    ui->statusbar->addWidget(ui->labelStatus);
    ui->textEditChat->verticalScrollBar()->installEventFilter(this);
} //initializeUi

bool MainWindow::isAppVisible()
{
    LOG_DEBUG(Q_FUNC_INFO);

    return !(isMinimized() || !isVisible() || !isActiveWindow());
} //isAppVisible

void MainWindow::connectSignals()
{
    LOG_DEBUG(Q_FUNC_INFO);

    connect(ui->textEditChat->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::onChatScrollBarChanged);

    connect(udpManager, &UdpChatSocketManager::messageReceived, this, [this](const QString &user, const QString &msg) {
        const QDateTime timestamp = QDateTime::currentDateTimeUtc();
        messageStore->insertMessage(user, msg, timestamp, false);

        bool isApplicationVisible = isAppVisible();
        if (!isApplicationVisible && !lastReadMarkerInserted && !isApplicationStarting) {
            redrawCurrentMessages();

            m_formatter->insertLastReadMarker(ui->textEditChat);
            lastReadMarkerInserted = true;
            ui->labelStatus->setText("You have unread messages!");
        }

        m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, false);

        ui->textEditChat->moveCursor(QTextCursor::End);

        if (!isAppVisible()) {
            QApplication::alert(this, 3000);
            new ToastNotification(QString("%1: %2").arg(user, msg), this);
        }
    });
} //connectSignals

void MainWindow::loadInitialState()
{
    LOG_DEBUG(Q_FUNC_INFO);

    settingsManager->load(configSettings);
    restoreGeometry(settingsManager->loadGeometry());

    loadQStyleSheetFolder();

    if (!styleRotator) {
        styleRotator = new StyleRotator(ui->comboBoxSelectStyleSheet, QStyleSheetMap, this);
    }

    setStyleSheet();
    setBackgroundImage();
    setAppWindowTitle();
    fillNetworkWidgets();
    updateUIWidgets();
} //loadInitialState

void MainWindow::initializeDatabase()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString dbPath = QCoreApplication::applicationDirPath() + QString("/chat_messages_instance_%1.db").arg(instanceID);

    messageStore = new MessageStore(dbPath, instanceID, this);

    if (!messageStore->open()) {
        qCritical() << "Unable to load message database.";
        return;
    }

    const QList<Message> messages = messageStore->fetchLastMessages(messagesPerPage);

    currentOffset = messageStore->messageCount() - messages.size();
    displayMessages(messages);
} //initializeDatabase

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->setupUi(this);

    isApplicationStarting = true;

    instanceID = acquireInstanceId();
    initializeManagers();
    initializeUi();
    connectSignals();
    loadInitialState();
    initializeDatabase();

    userNameSaveDebounceTimer.setInterval(500);
    userNameSaveDebounceTimer.setSingleShot(true);
    connect(&userNameSaveDebounceTimer, &QTimer::timeout, this, [this]() { settingsManager->save(configSettings); });

    QTimer::singleShot(5000, this, [this]() { isApplicationStarting = false; });

} //MainWindow

MainWindow::~MainWindow()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isDemoRunning)
        on_pushButtonStartStopDemo_clicked();

    if (settingsManager) {
        userNameSaveDebounceTimer.stop();
        settingsManager->save(configSettings);
        settingsManager->saveGeometry(saveGeometry());
    }

    releaseInstanceId(instanceID);

    delete ui;
    ui = nullptr;
} //~MainWindow

void MainWindow::changeEvent(QEvent *event)
{
    // LOG_DEBUG(Q_FUNC_INFO);

    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange || event->type() == QEvent::ActivationChange || event->type() == QEvent::Show) {
        if (!isApplicationStarting && isAppVisible()) {
            lastReadMarkerInserted = false;
        }
    }
} //changeEvent

QByteArray MainWindow::buildRawUdpPayload(const QString &user, const QString &msg) const
{
    LOG_DEBUG(Q_FUNC_INFO);

    return QString("%1 - %2").arg(user, msg).toLocal8Bit();
} //buildRawUdpPayload

bool MainWindow::sendUdpMessage(const QByteArray &data, const QHostAddress &address, quint16 port)
{
    LOG_DEBUG(Q_FUNC_INFO);

    return udpManager->sendMessage(data, address, port) == data.size();
} //sendUdpMessage

void MainWindow::storeAndDisplaySentMessage(const QString &user, const QString &msg, const QDateTime &timestamp)
{
    LOG_DEBUG(Q_FUNC_INFO);

    messageStore->insertMessage(user, msg, timestamp, true);
    m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, true);
} //storeAndDisplaySentMessage

void MainWindow::on_pushButtonSend_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!udpManager)
        return;

    const QString messageText = ui->lineEditChatText->text().trimmed();
    const QString userName = ui->lineEditUserName->text().trimmed();

    if (messageText.isEmpty() || userName.isEmpty())
        return;

    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());
    bool portOk = false;
    const quint16 port = ui->lineEditRemoteUDPPort->text().toUShort(&portOk);

    if (!portOk) {
        ui->labelStatus->setText("Invalid UDP port.");
        return;
    }

    const QDateTime timestamp = QDateTime::currentDateTimeUtc();
    const QByteArray rawData = buildRawUdpPayload(userName, messageText);

    if (sendUdpMessage(rawData, groupAddress, port)) {
        storeAndDisplaySentMessage(userName, messageText, timestamp);
        ui->lineEditChatText->clear();
    } else {
        const QString error = tr("%1 - ERROR writing to UDP socket: %2").arg(Q_FUNC_INFO, udpManager->lastError());
        ui->labelStatus->setText(error);
    }
} //on_pushButtonSend_clicked

QHostAddress MainWindow::parseLocalAddress() const
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString text = ui->comboBoxLocalUDPNetwork->currentText().trimmed();
    return (text == "ANY") ? QHostAddress(QHostAddress::AnyIPv4) : QHostAddress(text);
} //parseLocalAddress

bool MainWindow::bindUdpSockets(const QHostAddress &local, const QHostAddress &remote, quint16 port)
{
    LOG_DEBUG(Q_FUNC_INFO);

    bool recvBound = udpManager->bindReceiveSocket(local, remote, port);
    bool sendBound = udpManager->bindSendSocket(local);
    return recvBound && sendBound;
} //bindUdpSockets

void MainWindow::updateUiOnConnectSuccess()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->labelStatus->setText(tr("Connection established."));
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->frameUDPParameters->setEnabled(false);
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setCurrentIndex(0);

    ui->pushButtonStartStopDemo->setEnabled(!udpManager->isConnected());

} //updateUiOnConnectSuccess

void MainWindow::on_pushButtonConnect_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!udpManager)
        return;

    const QHostAddress localAddress = parseLocalAddress();
    const QHostAddress groupAddress = QHostAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    bool portOk = false;
    const quint16 port = ui->lineEditLocalUDPPort->text().toUShort(&portOk);
    if (!portOk || port == 0) {
        ui->labelStatus->setText(tr("Invalid local UDP port."));
        return;
    }

    if (bindUdpSockets(localAddress, groupAddress, port)) {
        updateUiOnConnectSuccess();
    } else {
        ui->labelStatus->setText(tr("Failed to bind one or both sockets."));
    }
} //on_pushButtonConnect_clicked

void MainWindow::on_lineEditChatText_returnPressed()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (ui->pushButtonSend->isEnabled()) {
        on_pushButtonSend_clicked();
    }
} //on_lineEditChatText_returnPressed

void MainWindow::resetUiAfterDisconnect()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false);

    ui->pushButtonStartStopDemo->setEnabled(!udpManager->isConnected());
} //resetUiAfterDisconnect

void MainWindow::on_pushButtonDisconnect_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (udpManager)
        udpManager->closeSockets();

    resetUiAfterDisconnect();
    ui->labelStatus->setText(tr("Disconnected from network."));
} //on_pushButtonDisconnect_clicked

void MainWindow::on_checkBoxMulticast_clicked(bool checked)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_multicast, checked);

} //on_checkBoxMulticast_clicked

void MainWindow::on_checkBoxLoopback_clicked(bool checked)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_loopback, checked);

} //on_checkBoxLoopback_clicked

void MainWindow::on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPAddress, arg1);

} //on_comboBoxLocalUDPNetwork_currentTextChanged

void MainWindow::on_lineEditLocalUDPPort_textChanged(const QString &arg1)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPPort, arg1.toUShort());

} //on_lineEditLocalUDPPort_textChanged

void MainWindow::on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPAddress, arg1);

} //on_lineEditRemoteUDPNetwork_textChanged

void MainWindow::on_lineEditRemoteUDPPort_textChanged(const QString &arg1)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPPort, arg1.toUShort());

} //on_lineEditRemoteUDPPort_textChanged

void MainWindow::on_spinBoxTTL_valueChanged(int arg1)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.udpTTL, arg1);

} //on_spinBoxTTL_valueChanged

void MainWindow::on_comboBoxSelectStyleSheet_currentTextChanged(const QString &styleName)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    const QString styleFile = QStyleSheetMap.value(styleName);
    settingsManager->update(configSettings.stylesheetName, styleFile);
    loadStyleSheet();

    if (!isDemoRunning) {
        redrawCurrentMessages(); // refresh visible messages with new theme context
        settingsManager->save(configSettings);
    }
} //on_comboBoxSelectStyleSheet_currentTextChanged

void MainWindow::populateStyleSheetMap(const QString &folderPath)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QDirIterator it(folderPath, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        QStyleSheetMap.insert(fileInfo.baseName(), fileInfo.absoluteFilePath());
    }
} //populateStyleSheetMap

void MainWindow::populateStyleSheetComboBox()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->comboBoxSelectStyleSheet->clear();
    QSignalBlocker block(ui->comboBoxSelectStyleSheet);
    ui->comboBoxSelectStyleSheet->addItem(""); // for 'none' or default
    ui->comboBoxSelectStyleSheet->addItems(QStyleSheetMap.keys());
} //populateStyleSheetComboBox

void MainWindow::on_checkBoxLoadStyleSheet_clicked(bool checked)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting) {
        return;
    }

    SettingsManager::update(configSettings.b_loadStyleSheet, checked);
} //on_checkBoxLoadStyleSheet_clicked

void MainWindow::loadQStyleSheetFolder()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString styleFolder = QApplication::applicationDirPath() + "/../QStyleSheets";
    QStyleSheetMap.clear(); // clear existing entries

    populateStyleSheetMap(styleFolder);
    populateStyleSheetComboBox();
} //loadQStyleSheetFolder

QString MainWindow::readStyleSheetFile(const QString &path)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QFile file(path);
    if (!file.exists() || !file.open(QFile::ReadOnly)) {
        qWarning() << "[readStyleSheetFile] Cannot read file:" << path;
        return {};
    }

    const QString content = QLatin1String(file.readAll());
    file.close();
    configSettings.stylesheetName = path;
    return content;
} //readStyleSheetFile

void MainWindow::redrawCurrentMessages()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QList<Message> messages = messageStore->fetchMessages(visibleOffset, visibleLimit);

    int savedScrollValue = ui->textEditChat->verticalScrollBar()->value();

    displayMessages(messages);

    ui->textEditChat->verticalScrollBar()->setValue(savedScrollValue);
} //redrawCurrentMessages

void MainWindow::loadStyleSheet()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString path = configSettings.stylesheetName;
    QString styleSheetContent = readStyleSheetFile(path);

    if (styleSheetContent.isEmpty()) {
        qApp->setStyleSheet("");
        QSignalBlocker block(ui->checkBoxLoadStyleSheet);
        ui->checkBoxLoadStyleSheet->setChecked(false);
        return;
    }

    QTimer::singleShot(0, this, [this, styleSheetContent]() {
        qApp->setStyleSheet(styleSheetContent);
        if (!isDemoRunning) {
            redrawCurrentMessages();
        } else {
            ui->textEditChat->verticalScrollBar()->setValue(ui->textEditChat->verticalScrollBar()->maximum());
            QString styleName = QStyleSheetMap.key(configSettings.stylesheetName);
            ui->labelStatus->setText(tr("Demo running - Applied stylesheet: %1").arg(styleName));
        }
    });
} //loadStyleSheet

void MainWindow::on_checkBoxDisplayBackgroundImage_clicked(bool checked)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_displayBackgroundImage, checked);
    settingsManager->save(configSettings);

    setBackgroundImage();
} //on_checkBoxDisplayBackgroundImage_clicked

void MainWindow::on_lineEditUserName_textChanged(const QString &newUserName)
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (isApplicationStarting)
        return;

    QString localNewUserName = newUserName.trimmed();

    if (localNewUserName.contains(" - ")) {
        localNewUserName.replace(" - ", "-");
        ui->labelStatus->setText("Note: ' - ' is not allowed in user name; replaced with '-'");

        // Reflect change in the UI without retriggering signal
        QSignalBlocker blocker(ui->lineEditUserName);
        ui->lineEditUserName->setText(localNewUserName);
    } else {
        ui->labelStatus->clear();
    }

    configSettings.userName = localNewUserName;
    setAppWindowTitle();
    userNameSaveDebounceTimer.start();
} //on_lineEditUserName_textChanged

QString MainWindow::generateNextTestMessage()
{
    LOG_DEBUG(Q_FUNC_INFO);

    static uint msgNumber = 1;
    return QString("Test %1").arg(msgNumber++);
} //generateNextTestMessage

void MainWindow::on_pushButtonTestMsg_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    ui->lineEditChatText->setText(generateNextTestMessage());
} //on_pushButtonTestMsg_clicked

void MainWindow::on_pushButtonDeleteDatabase_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                    tr("Confirm Clear"),
                                                                    tr("Are you sure you want to delete all chat messages?"),
                                                                    QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    if (messageStore->clearMessages()) {
        ui->textEditChat->clear();
        currentOffset = 0;
        ui->labelStatus->setText(tr("Chat history cleared."));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to clear chat history."));
    }
} //on_pushButtonDeleteDatabase_clicked

void MainWindow::startDemoModeUiSetup()
{
    LOG_DEBUG(Q_FUNC_INFO);

    isDemoRunning = true;
    styleRotator->start();

    ui->pushButtonStartStopDemo->setText("Stop Demo Mode");
    ui->labelStatus->setText("Demo Mode Running");

    ui->pushButtonConnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(false);
    ui->frameChatSend->setEnabled(false);

    ui->labelStatus->setText(tr("Running demo..."));
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setCurrentIndex(0);
} //startDemoModeUiSetup

void MainWindow::stopDemoModeUiReset()
{
    LOG_DEBUG(Q_FUNC_INFO);

    // demoSimulator->stopDemo();
    styleRotator->stop();

    ui->pushButtonStartStopDemo->setText("Start Demo Mode");
    ui->labelStatus->setText("Demo Mode Stopped");

    isDemoRunning = false;

    configSettings.stylesheetName = QStyleSheetMap.key(realStyleSheetName);
    ui->comboBoxSelectStyleSheet->setCurrentText(realStyleSheetName);
    setStyleSheet();
    // redrawCurrentMessages();
    realStyleSheetName.clear();

    ui->pushButtonConnect->setEnabled(true);
    ui->frameUDPParameters->setEnabled(true);
    ui->frameChatSend->setEnabled(true);

    ui->tabWidget->setTabEnabled(0, false);
} //stopDemoModeUiReset

void MainWindow::on_pushButtonStartStopDemo_clicked()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (!demoSimulator || !styleRotator || !udpManager)
        return;

    if (!isDemoRunning) {
        if (udpManager->isConnected()) {
            QMessageBox::warning(this, tr("Cannot Start Demo"), tr("Please disconnect from the UDP session before starting demo mode."));
            return;
        }

        if (realStyleSheetName.isEmpty()) {
            realStyleSheetName = QStyleSheetMap.key(configSettings.stylesheetName);
        }

        ui->textEditChat->clear();

        if (!demoSimulator->startDemo()) {
            QMessageBox::warning(this, tr("Demo Start Failed"), tr("No demo files found or all files are empty.\nPlease check the 'demofiles' folder."));
            return;
        }

        startDemoModeUiSetup();
    } else {
        demoSimulator->stopDemo();
        stopDemoModeUiReset();
    }
} //on_pushButtonStartStopDemo_clicked
