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

#include "mainwindow.h"
#include "../ToastNotification/toastnotification.h"
#include "ui_mainwindow.h"

#include "../Utils/debugmacros.h"

QString MainWindow::buildVersionSuffix(const QString &version) const
{
    LOG_DEBUG("");

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
} // buildVersionSuffix

void MainWindow::setAppWindowTitle()
{
    LOG_DEBUG("");

    const QString userName = configSettings.userName;
    const QString baseTitle = tr(APP_NAME)
                              + QString(" - %1_%2").arg(QString::number(instanceID), userName);

    const QString versionSuffix = buildVersionSuffix(tr(VERSION));

    setWindowTitle(baseTitle + versionSuffix);
} // setAppWindowTitle

QString MainWindow::buildAppVersionString() const
{
    LOG_DEBUG("");

    QString version = VERSION;

    if (version.contains("alpha", Qt::CaseInsensitive)
        || version.contains("bravo", Qt::CaseInsensitive)) {
        version += " " + tr(__TIME__); // Append compile time for dev builds
    }

    return version;
} // buildAppVersionString

QString MainWindow::detectCompilerInfo() const
{
    LOG_DEBUG("");

#ifdef ENV32
    return QString("Qt %1 - MinGW (32-bit)").arg(qVersion());
#else
    return QString("Qt %1 - MinGW (64-bit)").arg(qVersion());
#endif
} // detectCompilerInfo

QString MainWindow::buildAboutText(const QString &version,
                                   const QString &compileDate,
                                   const QString &releaseDate,
                                   const QString &compilerInfo) const
{
    LOG_DEBUG("");

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
} // buildAboutText

void MainWindow::on_actionAbout_triggered()
{
    LOG_DEBUG("");

    const QString version = buildAppVersionString();
    const QString compileDate = QStringLiteral(__DATE__); // e.g. "May 23 2025"
    const QString releaseDate = QLocale().toString(QDate::fromString(compileDate, "MMM dd yyyy"),
                                                   QLocale::ShortFormat);

    const QString compilerInfo = detectCompilerInfo();
    const QString aboutText = buildAboutText(version, compileDate, releaseDate, compilerInfo);

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr(APP_NAME));
    aboutBox.setText(aboutText);
    aboutBox.exec();
} // on_actionAbout_triggered

void MainWindow::on_actionAbout_Qt_triggered()
{
    LOG_DEBUG("");

    qApp->aboutQt();
} // on_actionAbout_Qt_triggered

bool MainWindow::copyResourceToFile(const QString &resourcePath, const QString &targetPath)
{
    LOG_DEBUG("");

    QFile resourceFile(resourcePath);
    QFile targetFile(targetPath);

    if (targetFile.exists()) {
        // Try to set permissive file mode before removal (especially for read-only files)
        if (!targetFile.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther
                                       | QFileDevice::WriteUser)) {
            qWarning().nospace() << "[copyResourceToFile] Cannot change permissions on: "
                                 << targetPath << " | Reason: " << targetFile.errorString();
        }

        if (!targetFile.remove()) {
            qCritical().nospace() << "[copyResourceToFile] Failed to remove: " << targetPath
                                  << " | Reason: " << targetFile.errorString();
            return false;
        }

        qDebug().nospace() << "[copyResourceToFile] Existing file removed: " << targetPath;
    } else {
        qDebug().nospace() << "[copyResourceToFile] No existing file to replace at: " << targetPath;
    }

    if (!resourceFile.copy(targetPath)) {
        qCritical().nospace() << "[copyResourceToFile] Copy failed: " << resourcePath << " → "
                              << targetPath << " | Reason: " << resourceFile.errorString();
        return false;
    }

    qDebug().nospace() << "[copyResourceToFile] Resource copied successfully to: " << targetPath;
    return true;
} // copyResourceToFile

void MainWindow::openResourceFile(const QString &fileName)
{
    LOG_DEBUG("");

    const QString resourcePath = QStringLiteral(":/resources") + fileName;
    const QString targetPath = qApp->applicationDirPath() + QDir::separator() + fileName;

    if (!copyResourceToFile(resourcePath, targetPath)) {
        qWarning().nospace() << "[openResourceFile] Failed to open resource file: " << targetPath;
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath))) {
        qWarning().nospace() << "[openResourceFile] Failed to launch file: " << targetPath;
    }
} // openResourceFile

void MainWindow::on_actionQt_License_triggered()
{
    LOG_DEBUG("");

    openResourceFile("/LGPLv3.pdf");
} // on_actionQt_License_triggered

void MainWindow::on_actionUser_Manual_triggered()
{
    LOG_DEBUG("");

    openResourceFile("/UserManual.pdf");
} // on_actionUser_Manual_triggered

bool MainWindow::isValidIPv4Address(const QString &ip) const
{
    // LOG_DEBUG("");

    return !ip.contains(":") && !ip.startsWith("169.254");
} // isValidIPv4Address

void MainWindow::fillNetworkWidgets()
{
    LOG_DEBUG("");

    ui->comboBoxLocalUDPNetwork->clear();
    ui->comboBoxLocalUDPNetwork->addItem("ANY");

    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : addresses) {
        const QString ip = address.toString();

        if (isValidIPv4Address(ip)) {
            ui->comboBoxLocalUDPNetwork->addItem(ip);
        }
    }
} // fillNetworkWidgets

void MainWindow::updateUIWidgets()
{
    LOG_DEBUG("");

    // Network Settings
    ui->lineEditUserName->setText(configSettings.userName);
    ui->checkBoxLoopback->setChecked(configSettings.b_loopback);
    // ui->checkBoxMulticast->setChecked(configSettings.b_multicast);
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
    QSignalBlocker block2(ui->comboBoxSelectBackground);
    ui->comboBoxSelectBackground->setCurrentText(configSettings.backgroundImageName);

    //Enable Test Features
    ui->checkBoxSpecialFeatures->setChecked(configSettings.b_enableTestFeatures);
    ui->tabWidget->setTabVisible(1,configSettings.b_enableTestFeatures); // Hide Special tab

    ui->spinBoxDaysToKeepLogs->setValue(configSettings.sessionLogRetentionDays);
} // updateUIWidgets

void MainWindow::setStyleSheet()
{
    LOG_DEBUG("");

    if (!configSettings.b_loadStyleSheet) {
        qDebug() << "[setStyleSheet] Skipping stylesheet load: setting disabled.";
        return;
    }

    const QString displayName = QStyleSheetMap.key(configSettings.stylesheetName);

    if (!displayName.isEmpty()) {
        QSignalBlocker block(ui->comboBoxSelectStyleSheet);
        ui->comboBoxSelectStyleSheet->setCurrentText(displayName);
        styleManager->loadStyleSheet(displayName);
    } else {
        qWarning() << "[setStyleSheet] Style sheet not found in map for file:"
                   << configSettings.stylesheetName;
    }
} // setStyleSheet

QString MainWindow::chatBackgroundStyle() const
{
    LOG_DEBUG("");
#ifdef AMC_VERSION
    return QStringLiteral("QTextEdit#textEditChat { "
                          "border-image: url(:/images/AMC_LOGO_DARK.png); "
                          "}");
#else
    return QStringLiteral("QTextEdit#textEditChat { "
                          "border-image: url(:/images/ChesterBGND.png); "
                          "}");
#endif
} // chatBackgroundStyle

QString MainWindow::chatBackgroundStyle(QString filePath) const
{
    LOG_DEBUG("");

    return QStringLiteral(
               "QTextEdit#textEditChat { "
               "border-image: url(\"%1\"); "
               "}")
        .arg(filePath);
} //chatBackgroundStyle

void MainWindow::setBackgroundImage(QString filePath)
{
    LOG_DEBUG("");

    if (configSettings.b_displayBackgroundImage) {
        QString styleSheetString = chatBackgroundStyle(filePath);
        qDebug() << styleSheetString;
        ui->textEditChat->setStyleSheet(styleSheetString);
    } else {
        ui->textEditChat->setStyleSheet(QString()); // reset to default
    }
} //setBackgroundImage

// void MainWindow::setBackgroundImage()
// {
//     LOG_DEBUG("");

//     if (configSettings.b_displayBackgroundImage) {
//         ui->textEditChat->setStyleSheet(chatBackgroundStyle());
//     } else {
//         ui->textEditChat->setStyleSheet(QString()); // reset to default
//     }
// } // setBackgroundImage

void MainWindow::displayMessages(const QList<Message> &messages)
{
    LOG_DEBUG("");

    // visibleOffset = currentOffset;
    // visibleLimit = messages.count();

    ui->textEditChat->clear();

    QString previousUser;
    for (const Message &msg : messages) {
        bool showUserName = (msg.user != previousUser);
        previousUser = msg.user;

        m_formatter->appendMessage(ui->textEditChat,
                                   showUserName ? msg.user : "",
                                   msg.text,
                                   msg.timestamp,
                                   msg.isSentByMe);
    }
} // displayMessages

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // LOG_DEBUG("");

    if (obj == ui->textEditChat->verticalScrollBar() && event->type() == QEvent::Type::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        const int delta = wheelEvent->angleDelta().y();
        const bool scrollingDown = delta < 0;

        if (chatPager)
            chatPager->handleScroll(ui->textEditChat->verticalScrollBar(), scrollingDown);
    }

    return QMainWindow::eventFilter(obj, event);
} // eventFilter

#ifdef EXPIRES
bool MainWindow::isTooOld()
{
    QFile thisApplication(QApplication::applicationFilePath());

    QFileInfo thisApplicationFileInfo(thisApplication);
    QString version(VERSION);

    if (version.contains("alpha")) {
        if (thisApplicationFileInfo.lastModified().addDays(ALPHA_TIME)
            < QDateTime::currentDateTime()) {
            displayIsTooOld(ALPHA_TIME,
                            thisApplicationFileInfo.lastModified().toString(
                                "MMM dd yyyy - hh:mm:ss"));
            return true;
        } else {
            return false;
        }
    } // if alpha
    else if (version.contains("beta")) {
        if (thisApplicationFileInfo.lastModified().addDays(BETA_TIME)
            < QDateTime::currentDateTime()) {
            displayIsTooOld(BETA_TIME,
                            thisApplicationFileInfo.lastModified().toString(
                                "MMM dd yyyy - hh:mm:ss"));
            return true;
        } else {
            return false;
        }
    } // if beta
    return false;
} // isTooOld

void MainWindow::displayIsTooOld(int days, QString fileDate)
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("Application Expired!");
    messageBox.setText("This application is older than " + QString::number(days) + " days - "
                       + fileDate
                       + " and has expired!\n\n"
                         "Application will now exit.");
    messageBox.exec();
} // displayIsTooOld
#endif

bool MainWindow::initializeManagers()
{
    LOG_DEBUG("");

    //instanceManager always first - it's a dependency
    instanceIdManager = std::make_unique<InstanceIdManager>(QCoreApplication::applicationDirPath());
    instanceID = instanceIdManager->acquire();

    if(instanceID < 1) return false;

    settingsManager = new SettingsManager(instanceID, QCoreApplication::applicationDirPath(), this);
    udpManager = new UdpChatSocketManager(this);
    m_formatter = new ChatFormatter(this);

    const QString dbPath = QCoreApplication::applicationDirPath()
                           + QString("/chat_messages_instance_%1.db").arg(instanceID);
    messageStore = new MessageStore(dbPath, instanceID, this);

    chatPager = std::make_unique<ChatPager>(messageStore, m_formatter, this);

    return true;
} // initializeManagers

void MainWindow::initializeUi()
{
    LOG_DEBUG("");

#ifndef ENABLE_DEMO_MODE
    ui->groupBoxDemoMode->hide();
#endif

    ui->pushButtonDisconnect->setEnabled(false);
    ui->checkBoxSpecialFeatures->setEnabled(false);
    ui->tabWidget->setTabEnabled(0, false); // Disable Chat tab
    ui->tabWidget->setTabEnabled(1, false); // Disable Test Features tab
    ui->tabWidget->setCurrentIndex(2);
    ui->statusbar->addWidget(ui->labelStatus);
    ui->textEditChat->verticalScrollBar()->installEventFilter(this);
} // initializeUi

void MainWindow::connectSignals()
{
    LOG_DEBUG("");

    // connect(udpManager,
    //         &UdpChatSocketManager::messageReceived,
    //         this,
    //         [this](const QString &user, const QString &msg) {
    //             const QDateTime timestamp = QDateTime::currentDateTimeUtc();
    //             messageStore->insertMessage(user, msg, timestamp, false);
    //             m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, false);
    //             ui->textEditChat->moveCursor(QTextCursor::End);

    //             if (isMinimized() || !isVisible() || !isActiveWindow()) {
    //                 QApplication::alert(this, 3000);
    //                 new ToastNotification(QString("%1: %2").arg(user, msg), this);
    //             }
    //         });
    connect(udpManager,
            &UdpChatSocketManager::messageReceived,
            this,
            [this](const QString &user, const QString &msg) {
                const QDateTime timestamp = QDateTime::currentDateTimeUtc();

                messageStore->insertMessage(user, msg, timestamp, false);
                m_formatter->appendMessage(
                    ui->textEditChat, user, msg, timestamp, false);

                if (chatSessionLogger.isActive()
                    && !chatSessionLogger.logMessage(
                        ChatSessionLogger::Direction::Received,
                        user,
                        msg,
                        timestamp)) {
                    ui->labelStatus->setText(
                        tr("Message received, but the session log could not "
                           "be updated: %1")
                            .arg(chatSessionLogger.lastError()));
                }

                ui->textEditChat->moveCursor(QTextCursor::End);

                if (isMinimized() || !isVisible() || !isActiveWindow()) {
                    QApplication::alert(this, 3000);
                    new ToastNotification(
                        QString("%1: %2").arg(user, msg), this);
                }
            });

    connect(this, &MainWindow::signalRequestTabSwitchToChat, this, [this]() {
        if (!ui || !ui->tabWidget)
            return;

        ui->tabWidget->setTabEnabled(0, true);
        ui->tabWidget->setCurrentIndex(0);
    });

    connect(this,
            &MainWindow::signalRequestRedrawCurrentMessages,
            this,
            &MainWindow::redrawCurrentMessages);

    connect(chatPager.get(), &ChatPager::messagesReady, this, &MainWindow::displayMessages);

    connect(chatPager.get(), &ChatPager::scrollToTopAdjustmentRequested, this, [this]() {
        QScrollBar *sb = ui->textEditChat->verticalScrollBar();
        if (sb)
            sb->setValue(sb->minimum() + 1);
    });
    connect(chatPager.get(), &ChatPager::scrollToBottomAdjustmentRequested, this, [this]() {
        QScrollBar *sb = ui->textEditChat->verticalScrollBar();
        if (sb)
            sb->setValue(sb->maximum());
    });
    connect(chatPager.get(), &ChatPager::scrollToValueAdjustmentRequested, this, [this](int value) {
        QScrollBar *sb = ui->textEditChat->verticalScrollBar();
        if (sb)
            sb->setValue(value);
    });

} // connectSignals

void MainWindow::loadInitialState()
{
    LOG_DEBUG("");

    settingsManager->load(configSettings);
    restoreGeometry(settingsManager->loadGeometry());

    loadQStyleSheetFolder();
    loadBackgroundImageFolder();

#ifdef ENABLE_DEMO_MODE
    if (!styleRotator) {
        styleRotator = new StyleRotator(ui->comboBoxSelectStyleSheet, QStyleSheetMap, this);
    }
#endif

    setStyleSheet();
    setBackgroundImage(backgroundImageMap.value(configSettings.backgroundImageName));
    setAppWindowTitle();
    fillNetworkWidgets();
    updateUIWidgets();
} // loadInitialState

void MainWindow::initializeDatabase()
{
    LOG_DEBUG("");

    if (!messageStore->open()) {
        qCritical() << "Unable to load message database.";
        return;
    }

    const QList<Message> messages = messageStore->fetchLastMessages(chatPager->messagesPerPage());

    chatPager->loadPage(messageStore->messageCount());

    displayMessages(messages);
} // initializeDatabase

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    LOG_DEBUG("");

    ui->setupUi(this);

#ifdef EXPIRES
    if (isTooOld()) {
        exit(0);
    }
#endif

    if (!initializeManagers())
        return;

    // Continue the rest of startup...
    m_startupSucceeded = true;
    isApplicationStarting = true;

    styleManager = std::make_unique<StyleManager>();

    initializeUi();
    connectSignals();
    loadInitialState();
    initializeDatabase();

    userNameSaveDebounceTimer.setInterval(500);
    userNameSaveDebounceTimer.setSingleShot(true);
    connect(&userNameSaveDebounceTimer, &QTimer::timeout, this, [this]() {
        settingsManager->save(configSettings);
    });
    autoTestMessageTimer.setInterval(1000);
    autoTestMessageTimer.setSingleShot(false);
    autoTestMessageTimer.setTimerType(Qt::PreciseTimer);

    connect(&autoTestMessageTimer, &QTimer::timeout, this, &MainWindow::sendAutoTestMessage);


    if (!chatSessionLogger.purgeOldLogs(
            QCoreApplication::applicationDirPath(),
            configSettings.sessionLogRetentionDays)) {
        LOG_DEBUG(chatSessionLogger.lastError());
    }

    isApplicationStarting = false;
} // MainWindow

MainWindow::~MainWindow()
{
    LOG_DEBUG("");
    chatSessionLogger.endSession();

#ifdef ENABLE_DEMO_MODE
    if (isDemoRunning)
        on_pushButtonStartStopDemo_clicked();
#endif

    if (settingsManager) {
        userNameSaveDebounceTimer.stop();
        settingsManager->save(configSettings);
        settingsManager->saveGeometry(saveGeometry());
    }

    if (instanceID > 0 && instanceIdManager)
        instanceIdManager->release(instanceID);


    delete ui;
    ui = nullptr;
} // ~MainWindow

QByteArray MainWindow::buildRawUdpPayload(const QString &user, const QString &msg) const
{
    LOG_DEBUG("");
    QByteArray returnArray = ui->checkBoxIncludeName->isChecked()
                                 ? QString("%1 - %2").arg(user, msg).toUtf8()
                                 : QString("%2").arg(msg).toUtf8();
    return returnArray;
} // buildRawUdpPayload

bool MainWindow::sendUdpMessage(const QByteArray &data, const QHostAddress &address, quint16 port)
{
    LOG_DEBUG("");

    return udpManager->sendMessage(data, address, port) == data.size();
} // sendUdpMessage

void MainWindow::storeAndDisplaySentMessage(const QString &user,
                                            const QString &msg,
                                            const QDateTime &timestamp)
{
    LOG_DEBUG("");

    messageStore->insertMessage(user, msg, timestamp, true);
    m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, true);

    if (chatSessionLogger.isActive()
        && !chatSessionLogger.logMessage(
            ChatSessionLogger::Direction::Sent,
            user,
            msg,
            timestamp)) {
        ui->labelStatus->setText(
            tr("Message sent, but the session log could not be updated: %1")
                .arg(chatSessionLogger.lastError()));
    }
} // storeAndDisplaySentMessage

void MainWindow::on_pushButtonSend_clicked()
{
    LOG_DEBUG("");

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
        const QString error = tr("%1 - ERROR writing to UDP socket: %2")
                                  .arg(Q_FUNC_INFO, udpManager->lastError());
        ui->labelStatus->setText(error);
    }
} // on_pushButtonSend_clicked

void MainWindow::updateUiOnConnectSuccess()
{
    LOG_DEBUG("");

    ui->labelStatus->setText(tr("Connection established."));
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->frameUDPParameters->setEnabled(false);
    ui->checkBoxSpecialFeatures->setEnabled(true);
    QTimer::singleShot(0, this, [this]() { emit signalRequestTabSwitchToChat(); });

    ui->tabWidget->setTabEnabled(1,true);
    ui->tabWidget->setTabVisible(1,configSettings.b_enableTestFeatures);
} // updateUiOnConnectSuccess

QHostAddress MainWindow::parseLocalAddress() const
{
    LOG_DEBUG("");

    const QString text = ui->comboBoxLocalUDPNetwork->currentText().trimmed();
    return (text == "ANY") ? QHostAddress(QHostAddress::AnyIPv4) : QHostAddress(text);
} // parseLocalAddress

bool MainWindow::bindUdpSockets(const QHostAddress &local, const QHostAddress &remote, quint16 port)
{
    LOG_DEBUG("");

    bool recvBound = udpManager->bindReceiveSocket(local, remote, port);
    bool sendBound = udpManager->bindSendSocket(local);
    return recvBound && sendBound;
} // bindUdpSockets

void MainWindow::on_pushButtonConnect_clicked()
{
    LOG_DEBUG("");

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

    bool remotePortOk = false;
    const quint16 remotePort = ui->lineEditRemoteUDPPort->text().toUShort(&remotePortOk);

    if (!remotePortOk || remotePort == 0) {
        ui->labelStatus->setText(tr("Invalid remote UDP port."));
        return;
    }

    // if (bindUdpSockets(localAddress, groupAddress, port)) {
    //     updateUiOnConnectSuccess();
    // } else {
    //     ui->labelStatus->setText(tr("Failed to bind one or both sockets."));
    // }

    if (bindUdpSockets(localAddress, groupAddress, port)) {
        updateUiOnConnectSuccess();

        if (!chatSessionLogger.startSession(
                QCoreApplication::applicationDirPath(),
                instanceID,
                ui->lineEditUserName->text().trimmed(),
                localAddress.toString(),
                port,
                groupAddress.toString(),
                remotePort)) {
            QMessageBox::warning(
                this,
                tr("Chat Session Log"),
                tr("The network connection succeeded, but the session log "
                   "could not be created:\n\n%1")
                    .arg(chatSessionLogger.lastError()));
        }
    } else {
        ui->labelStatus->setText(tr("Failed to bind one or both sockets."));
    }
} // on_pushButtonConnect_clicked

void MainWindow::on_lineEditChatText_returnPressed()
{
    LOG_DEBUG("");

    if (ui->pushButtonSend->isEnabled()) {
        on_pushButtonSend_clicked();
    }
} // on_lineEditChatText_returnPressed

void MainWindow::resetUiAfterDisconnect()
{
    LOG_DEBUG("");

    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false);
    // ui->checkBoxSpecialFeatures->setChecked(configSettings.b_enableTestFeatures);
    ui->checkBoxSpecialFeatures->setEnabled(false);
    ui->tabWidget->setTabEnabled(1,false);// disable Special tab
    // ui->pushButtonStartStopDemo->setEnabled(!udpManager->isConnected());
} // resetUiAfterDisconnect

void MainWindow::on_pushButtonDisconnect_clicked()
{
    LOG_DEBUG("");

    stopAutoTestMessages();

    if (udpManager)
        udpManager->closeSockets();

    chatSessionLogger.endSession();
    resetUiAfterDisconnect();
    ui->labelStatus->setText(tr("Disconnected from network."));
} // on_pushButtonDisconnect_clicked

void MainWindow::on_checkBoxMulticast_clicked(bool checked)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    // SettingsManager::update(configSettings.b_multicast, checked);

} // on_checkBoxMulticast_clicked

void MainWindow::on_checkBoxLoopback_clicked(bool checked)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_loopback, checked);

} // on_checkBoxLoopback_clicked

void MainWindow::on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPAddress, arg1);

} // on_comboBoxLocalUDPNetwork_currentTextChanged

void MainWindow::on_lineEditLocalUDPPort_textChanged(const QString &arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPPort, arg1.toUShort());

} // on_lineEditLocalUDPPort_textChanged

void MainWindow::on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPAddress, arg1);

    QHostAddress groupAddress(arg1);

    if(groupAddress.isMulticast())
        ui->comboBoxLocalUDPNetwork->setCurrentText("ANY");

} // on_lineEditRemoteUDPNetwork_textChanged

void MainWindow::on_lineEditRemoteUDPPort_textChanged(const QString &arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPPort, arg1.toUShort());

} // on_lineEditRemoteUDPPort_textChanged

void MainWindow::on_spinBoxTTL_valueChanged(int arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.udpTTL, arg1);

} // on_spinBoxTTL_valueChanged

void MainWindow::on_comboBoxSelectStyleSheet_currentTextChanged(const QString &styleName)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    if (styleName.isEmpty()) {
        ui->checkBoxLoadStyleSheet->setChecked(false);
        settingsManager->update(configSettings.b_loadStyleSheet, false);
    }

    const QString styleFile = QStyleSheetMap.value(styleName);
    settingsManager->update(configSettings.stylesheetName, styleFile);
    styleManager->loadStyleSheet(styleName);

#ifdef ENABLE_DEMO_MODE
    if (!isDemoRunning) {
        redrawCurrentMessages(); // refresh visible messages with new theme context
        settingsManager->save(configSettings);
    }
#else
    redrawCurrentMessages(); // refresh visible messages with new theme context
    settingsManager->save(configSettings);
#endif
} // on_comboBoxSelectStyleSheet_currentTextChanged

void MainWindow::populateStyleSheetMap(const QString &folderPath)
{
    LOG_DEBUG("");

    QDirIterator it(folderPath, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        QStyleSheetMap.insert(fileInfo.baseName(), fileInfo.absoluteFilePath());
    }
} // populateStyleSheetMap

void MainWindow::populateStyleSheetComboBox()
{
    LOG_DEBUG("");

    ui->comboBoxSelectStyleSheet->clear();
    QSignalBlocker block(ui->comboBoxSelectStyleSheet);
    ui->comboBoxSelectStyleSheet->addItem(""); // for 'none' or default
    ui->comboBoxSelectStyleSheet->addItems(QStyleSheetMap.keys());
} // populateStyleSheetComboBox

void MainWindow::on_checkBoxLoadStyleSheet_clicked(bool checked)
{
    LOG_DEBUG("");

    if (isApplicationStarting) {
        return;
    }

    SettingsManager::update(configSettings.b_loadStyleSheet, checked);
} // on_checkBoxLoadStyleSheet_clicked

void MainWindow::loadQStyleSheetFolder()
{
    LOG_DEBUG("");

    const QString styleFolder = QApplication::applicationDirPath() + "/../QStyleSheets";
    QStyleSheetMap.clear(); // clear existing entries

    populateStyleSheetMap(styleFolder);
    populateStyleSheetComboBox();
    styleManager->setStyleSheetMap(QStyleSheetMap);
} // loadQStyleSheetFolder

void MainWindow::redrawCurrentMessages()
{
    LOG_DEBUG("");

    const QList<Message> messages = messageStore->fetchLastMessages(chatPager->messagesPerPage());

    displayMessages(messages);
} // redrawCurrentMessages

void MainWindow::loadBackgroundImageFolder()
{
    QString QStyleSheetFolder = QApplication::applicationDirPath() + "/../QStyleSheets/backgrounds";
    QDirIterator fileIterator(QStyleSheetFolder, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

    while (fileIterator.hasNext()) {
        QFile styleSheetFile(fileIterator.next());
        QFileInfo fileInfo(styleSheetFile);
        backgroundImageMap.insert(fileInfo.baseName(), styleSheetFile.fileName());
    }

    ui->comboBoxSelectBackground->clear();
    ui->comboBoxSelectBackground->addItem("");
    ui->comboBoxSelectBackground->addItems(backgroundImageMap.keys());
} // loadBackgroundImageFolder

void MainWindow::on_checkBoxDisplayBackgroundImage_clicked(bool checked)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_displayBackgroundImage, checked);
    settingsManager->save(configSettings);

    setBackgroundImage(backgroundImageMap.value(configSettings.backgroundImageName));
} // on_checkBoxDisplayBackgroundImage_clicked

void MainWindow::on_lineEditUserName_textChanged(const QString &newUserName)
{
    LOG_DEBUG("");

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
} // on_lineEditUserName_textChanged

QString MainWindow::generateNextTestMessage()
{
    LOG_DEBUG("");

    // static uint msgNumber = 1;
    return QString("Test %1").arg(msgNumber++);
} // generateNextTestMessage

QStringList MainWindow::buildIncrementalPayloads(int firstSize, int lastSize)
{
    QStringList payloads;

    if (firstSize < 1 || lastSize < firstSize) {
        return payloads;
    }

    payloads.reserve(lastSize - firstSize + 1);

    for (int size = firstSize; size <= lastSize; ++size) {
        // Matches your test pattern:
        // 1  -> "1"
        // 2  -> "22"
        // 3  -> "333"
        // ...
        // 9  -> "999999999"
        // 10 -> "0000000000"
        // 11 -> "11111111111"
        const QChar fillChar = QChar(u'0' + (size % 10));

        payloads.append(QString(size, fillChar));
    }

    return payloads;
} // buildIncrementalPayloads

QStringList MainWindow::buildRandomIncrementalPayloads(int firstSize, int lastSize)
{
    static const QString customBase64Chars = QStringLiteral(
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");

    QStringList payloads;

    if (firstSize < 1 || lastSize < firstSize || customBase64Chars.isEmpty()) {
        return payloads;
    }

    payloads.reserve(lastSize - firstSize + 1);

    auto *rng = QRandomGenerator::global();

    for (int size = firstSize; size <= lastSize; ++size) {
        QString payload;
        payload.reserve(size);

        for (int i = 0; i < size; ++i) {
            const int charIndex = rng->bounded(customBase64Chars.size());
            payload.append(customBase64Chars.at(charIndex));
        }

        payloads.append(payload);
    }

    return payloads;
} // buildRandomIncrementalPayloads

void MainWindow::sendIncrementalMessageSizes(int firstSize, int lastSize)
{
    LOG_DEBUG("");
    QStringList payloads;
    const QString userName = ui->lineEditUserName->text().trimmed();

    payloads = ui->checkBoxRandomText->isChecked()
                   ? buildRandomIncrementalPayloads(firstSize, lastSize)
                   : buildIncrementalPayloads(firstSize, lastSize);

    for (const QString &payload : payloads) {
        // qDebug().noquote() << payload.length() << payload;

        if (!udpManager)
            return;

        const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());
        bool portOk = false;
        const quint16 port = ui->lineEditRemoteUDPPort->text().toUShort(&portOk);

        if (!portOk) {
            ui->labelStatus->setText("Invalid UDP port.");
            return;
        }

        const QDateTime timestamp = QDateTime::currentDateTimeUtc();
        const QByteArray rawData = buildRawUdpPayload(userName, payload.toLocal8Bit());

        if (sendUdpMessage(rawData, groupAddress, port)) {
            storeAndDisplaySentMessage(userName, rawData, timestamp);
            QApplication::processEvents();
            QThread::msleep(10);
        } else {
            const QString error = tr("%1 - ERROR writing to UDP socket: %2")
                                      .arg(Q_FUNC_INFO, udpManager->lastError());
            ui->labelStatus->setText(error);
        }
    }
} //sendIncrementalMessageSizes

int MainWindow::autoTestMessageIntervalMs() const
{
    bool ok = false;
    const uint intervalValue = ui->lineEditAutoSchedule->text().trimmed().toUInt(&ok);

    if (!ok || intervalValue == 0)
        return 0;

    if (ui->radioButtonAutoMinutes->isChecked())
        return static_cast<int>(intervalValue * 60000);

    return static_cast<int>(intervalValue * 1000);
} //autoTestMessageIntervalMs

void MainWindow::sendAutoTestMessage()
{
    if (!udpManager || !udpManager->isConnected()) {
        stopAutoTestMessages();
        ui->labelStatus->setText(tr("Auto test stopped: UDP is not connected."));
        return;
    }

    const QString userName = ui->lineEditUserName->text().trimmed();

    if (userName.isEmpty()) {
        stopAutoTestMessages();
        ui->labelStatus->setText(tr("Auto test stopped: user name is empty."));
        return;
    }

    const QString messageText = generateNextTestMessage();

    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    bool portOk = false;
    const quint16 port = ui->lineEditRemoteUDPPort->text().toUShort(&portOk);

    if (!portOk || port == 0) {
        stopAutoTestMessages();
        ui->labelStatus->setText(tr("Auto test stopped: invalid UDP port."));
        return;
    }

    const QDateTime timestamp = QDateTime::currentDateTimeUtc();
    const QByteArray rawData = buildRawUdpPayload(userName, messageText);

    if (sendUdpMessage(rawData, groupAddress, port)) {
        storeAndDisplaySentMessage(userName, messageText, timestamp);
        ui->textEditChat->moveCursor(QTextCursor::End);
    } else {
        const QString error = tr("%1 - ERROR writing to UDP socket: %2")
        .arg(Q_FUNC_INFO, udpManager->lastError());

        ui->labelStatus->setText(error);
        stopAutoTestMessages();
    }
} //sendAutoTestMessage

void MainWindow::stopAutoTestMessages()
{
    autoTestMessageTimer.stop();

    ui->pushButtonStartAutoSchedule->setEnabled(true);
    ui->pushButtonStopAutoSchedule->setEnabled(false);

    ui->radioButtonAutoSeconds->setEnabled(true);
    ui->radioButtonAutoMinutes->setEnabled(true);
    ui->lineEditAutoSchedule->setEnabled(true);
} //stopAutoTestMessages

void MainWindow::on_pushButtonStartAutoSchedule_clicked()
{
    const int intervalMs = autoTestMessageIntervalMs();

    if (intervalMs <= 0) {
        ui->labelStatus->setText(tr("Invalid auto test interval."));
        return;
    }

    if (!udpManager || !udpManager->isConnected()) {
        ui->labelStatus->setText(tr("Connect UDP before starting auto test messages."));
        return;
    }

    ui->pushButtonStartAutoSchedule->setEnabled(false);
    ui->pushButtonStopAutoSchedule->setEnabled(true);

    ui->radioButtonAutoSeconds->setEnabled(false);
    ui->radioButtonAutoMinutes->setEnabled(false);
    ui->lineEditAutoSchedule->setEnabled(false);

    autoTestMessageTimer.start(intervalMs);

    sendAutoTestMessage();   // Sends the first one immediately.
    ui->tabWidget->setCurrentIndex(0);

} //on_pushButtonStartAutoSchedule_clicked

void MainWindow::on_pushButtonStopAutoSchedule_clicked()
{
    stopAutoTestMessages();
} //on_pushButtonStopAutoSchedule_clicked

void MainWindow::on_pushButtonTestMsg_clicked()
{
    LOG_DEBUG("");
    ui->lineEditChatText->setText(generateNextTestMessage());

    // if(ui->checkBoxIncrementSize->isChecked()){
    //     sendIncrementalMessageSizes();
    // }
    // else {
    // ui->lineEditChatText->setText(generateNextTestMessage());
    // }
} //on_pushButtonTestMsg_clicked

void MainWindow::on_pushButtonDeleteDatabase_clicked()
{
    LOG_DEBUG("");

    const QMessageBox::StandardButton reply
        = QMessageBox::question(this,
                                tr("Confirm Clear"),
                                tr("Are you sure you want to delete all chat messages?"),
                                QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    if (messageStore->clearMessages()) {
        ui->textEditChat->clear();

        ui->labelStatus->setText(tr("Chat history cleared."));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to clear chat history."));
    }
} //on_pushButtonDeleteDatabase_clicked

#ifdef ENABLE_DEMO_MODE
void MainWindow::stopDemoModeUiReset()
{
    LOG_DEBUG("");

    if (demoSimulator) {
        demoSimulator->stopDemo();
        demoSimulator.reset(); // delete and nullify
    }

    styleRotator->stop();

    ui->pushButtonStartStopDemo->setEnabled(false);
    ui->pushButtonStartStopDemo->setText("Start Demo Mode");

    QTimer::singleShot(5000, this, [this]() {
        if (ui->pushButtonConnect->isEnabled())            //if not connected
            ui->pushButtonStartStopDemo->setEnabled(true); //enable demo button again
    });

    ui->labelStatus->setText("Demo Mode Stopped");

    configSettings.stylesheetName = QStyleSheetMap.key(realStyleSheetName);
    ui->comboBoxSelectStyleSheet->setCurrentText(realStyleSheetName);
    realStyleSheetName.clear();

    isDemoRunning = false;

    const QList<Message> messages = messageStore->fetchLastMessages(chatPager->messagesPerPage());
    chatPager->loadPage(messageStore->messageCount() - messages.size());

    ui->pushButtonConnect->setEnabled(true);
    ui->frameUDPParameters->setEnabled(true);
    ui->frameChatSend->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false);
} //stopDemoModeUiReset

void MainWindow::startDemoModeUiSetup()
{
    LOG_DEBUG("");

    isDemoRunning = true;

    ui->textEditChat->clear();

    styleRotator->start();

    ui->pushButtonStartStopDemo->setText("Stop Demo Mode");
    ui->labelStatus->setText(tr("Running demo..."));

    ui->pushButtonConnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(false);
    ui->frameChatSend->setEnabled(false);

    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setCurrentIndex(0);
    // chatPager->loadPage(messageStore->messageCount());

} //startDemoModeUiSetup

void MainWindow::on_pushButtonStartStopDemo_clicked()
{
    LOG_DEBUG("");

    if (!styleRotator || !udpManager)
        return;

    if (!isDemoRunning) {
        if (udpManager->isConnected()) {
            QMessageBox::warning(
                this,
                tr("Cannot Start Demo"),
                tr("Please disconnect from the UDP session before starting demo mode."));
            return;
        }

        if (realStyleSheetName.isEmpty()) {
            realStyleSheetName = QStyleSheetMap.key(configSettings.stylesheetName);
        }

        // Always create a new simulator for fresh state
        demoSimulator.reset(new DemoChatSimulator(ui->textEditChat, m_formatter, this));

        connect(demoSimulator.get(),
                &DemoChatSimulator::signalRequestClearChatDisplay,
                this,
                [this]() {
                    QTimer::singleShot(0, this, [this]() {
                        if (ui && ui->textEditChat)
                            ui->textEditChat->clear(); // Safe on main thread
                    });
                });

        if (!demoSimulator->startDemo()) {
            demoSimulator.reset(); // destroy faulty instance
            QMessageBox::warning(this,
                                 tr("Demo Start Failed"),
                                 tr("No demo files found or all files are empty.\nPlease check the "
                                    "'demofiles' folder."));
            return;
        }

        startDemoModeUiSetup();

    } else {
        stopDemoModeUiReset(); // calls stopDemo() and deletes demoSimulator inside
    }
} //on_pushButtonStartStopDemo_clicked
#endif

void MainWindow::on_pushButtonResetMsgSeq_clicked()
{
    msgNumber = 1;
} //on_pushButtonResetMsgSeq_clicked

void MainWindow::on_checkBoxSpecialFeatures_clicked(bool checked)
{
    LOG_DEBUG("");

    if (!udpManager)
        return;

    configSettings.b_enableTestFeatures = checked;
    settingsManager->save(configSettings);

    ui->tabWidget->setTabVisible(1, checked);

    checked ? ui->tabWidget->setCurrentIndex(1) : ui->tabWidget->setCurrentIndex(2);
} // on_checkBoxSpecialFeatures_clicked

void MainWindow::on_pushButtonStartIncrementalMessages_clicked()
{
    sendIncrementalMessageSizes(ui->lineEditMinSize->text().toUInt(),
                                ui->lineEditMaxSize->text().toUInt());
    ui->tabWidget->setCurrentIndex(0);
} //on_pushButtonStartIncrementalMessages_clicked

// void MainWindow::on_pushButtonStartAutoSchedule_clicked()
// {
//     ui->pushButtonStartAutoSchedule->setEnabled(false);
//     ui->pushButtonStopAutoSchedule->setEnabled(true);
// } //

// void MainWindow::on_pushButtonStopAutoSchedule_clicked()
// {
//     ui->pushButtonStartAutoSchedule->setEnabled(true);
//     ui->pushButtonStopAutoSchedule->setEnabled(false);
// } //


void MainWindow::showBackgroundImage(QString filePath)
{
    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        QSize targetSize(200, 200);
        QPixmap scaledPixmap = pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelTestBackgroundImage->setPixmap(scaledPixmap);
        ui->labelTestBackgroundImage->setFixedSize(targetSize); // Optional: force label size
    } else {
        ui->labelTestBackgroundImage->clear();
    }
} //showBackgroundImage

void MainWindow::on_comboBoxSelectBackground_currentTextChanged(const QString &arg1)
{
    LOG_DEBUG("");

    if (isApplicationStarting)
        return;

    if(arg1.isEmpty()){
        ui->checkBoxDisplayBackgroundImage->setChecked(false);
        settingsManager->update(configSettings.b_displayBackgroundImage, false);
    }

    const QString image = backgroundImageMap.value(arg1);
    settingsManager->update(configSettings.backgroundImageName, arg1);

#ifdef ENABLE_DEMO_MODE
    if (!isDemoRunning) {
        redrawCurrentMessages(); // refresh visible messages with new theme context
        settingsManager->save(configSettings);
    }
#else
    redrawCurrentMessages(); // refresh visible messages with new theme context
    settingsManager->save(configSettings);
#endif

    QString filePath = backgroundImageMap.value(arg1);
    showBackgroundImage(filePath);
    setBackgroundImage(filePath);

    // if (isApplicationStarting)
    //     return;

    // configSettings.styleSheetSettings.backgroundImageName = arg1;
    // configSettings.writeSettings();

    // QString filePath = backgroundImageMap.value(arg1);
    // showBackgroundImage(filePath);
    // setBackgroundImage(filePath);
} //on_comboBoxSelectBackground_currentTextChanged

void MainWindow::on_spinBoxDaysToKeepLogs_valueChanged(int arg1)
{
    settingsManager->update(configSettings.sessionLogRetentionDays, arg1);
    settingsManager->save(configSettings);

} //on_spinBoxDaysToKeepLogs_valueChanged
