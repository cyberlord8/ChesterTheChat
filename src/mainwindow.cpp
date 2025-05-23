#include "mainwindow.h"
#include "ui_mainwindow.h"

QString MainWindow::buildVersionSuffix(const QString &version) const
{
    if (version.contains("alpha", Qt::CaseInsensitive)) {
        QString buildDate = tr(BUILDDATE);
        int lastColonIndex = buildDate.lastIndexOf(':');
        if (lastColonIndex != -1) {
            buildDate = buildDate.left(lastColonIndex);
        }
        return QString(" - %1 - %2").arg(version, buildDate);
    } else if (version.contains("beta", Qt::CaseInsensitive)) {
        return QString(" - %1 - %2").arg(version, tr(RELEASEDATE));
    } else {
        return QString(" - %1").arg(tr(RELEASEDATE));
    }
} //buildVersionSuffix

void MainWindow::setAppWindowTitle()
{
    const QString userName = configSettings.userName;
    const QString baseTitle = tr(APP_NAME) + QString(" - %1_%2").arg(QString::number(instanceID), userName);
    const QString version = tr(VERSION);

    const QString versionSuffix = buildVersionSuffix(version);

    setWindowTitle(baseTitle + versionSuffix);
} //setAppWindowTitle

QString MainWindow::getInstanceIdsFilePath() const
{
    return QCoreApplication::applicationDirPath() + "/instance_ids.txt";
} //getInstanceIdsFilePath

QString MainWindow::getInstanceIdsLockFilePath() const
{
    return QCoreApplication::applicationDirPath() + "/instance_ids.lock";
} //getInstanceIdsLockFilePath

QSet<int> MainWindow::loadInstanceIdsExcluding(int excludeId, const QString &filePath) const
{
    QSet<int> ids;
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            int id = in.readLine().toInt();
            if (excludeId < 0 || id != excludeId)
                ids.insert(id);
        }
        file.close();
    }
    return ids;
} //loadInstanceIdsExcluding

void MainWindow::saveInstanceIds(const QSet<int> &ids, const QString &filePath) const
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        for (int id : std::as_const(ids)) {
            out << id << "\n";
        }
        file.close();
    }
} //saveInstanceIds

void MainWindow::releaseInstanceId(int instanceId)
{
    if (instanceId < 1)
        return;

    const QString idsFilePath = getInstanceIdsFilePath();
    const QString lockFilePath = getInstanceIdsLockFilePath();

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt for release");
        return;
    }

    QSet<int> usedIds = loadInstanceIdsExcluding(instanceId, idsFilePath);
    saveInstanceIds(usedIds, idsFilePath);

    lock.unlock();
} //releaseInstanceId

int MainWindow::acquireInstanceId()
{
    const QString idsFilePath = getInstanceIdsFilePath();
    const QString lockFilePath = getInstanceIdsLockFilePath();

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt");
        return -1;
    }

    QSet<int> usedIds = loadInstanceIdsExcluding(-1, idsFilePath); // -1 means load all

    // Find the first unused positive ID
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
    QString version = VERSION;
    if (version.contains("alpha", Qt::CaseInsensitive) || version.contains("bravo", Qt::CaseInsensitive)) {
        version += " " + tr(__TIME__); // Append compile time
    }
    return version;
} //buildAppVersionString

QString MainWindow::detectCompilerInfo() const
{
#ifdef ENV32
    return QString("Qt %1 MinGW (32-bit)").arg(qVersion());
#else
    return QString("Qt %1 MinGW (64-bit)").arg(qVersion());
#endif
} //detectCompilerInfo

QString MainWindow::buildAboutText(const QString &version, const QString &compileDate, const QString &releaseDate, const QString &compilerInfo) const
{
    QString about;
    about += tr(APP_LICENSE) + "\n";
    about += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    about += QString(" - %1 %2 %3\n").arg(COPYRIGHT, compileDate, COMPANY);
    about += QString(" - %1\n").arg(RIGHTS);
    about += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    about += QString(" - %1%2\n").arg(tr("Version: "), version);
    about += QString(" - %1%2\n").arg(tr("Release date: "), releaseDate);
    about += QString(" - %1%2\n").arg(tr("Compiler: "), compilerInfo);
    about += QString(" - %1%2\n").arg(tr("Running on: "), SYSTEMINFO);
    about += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    about += QString("%1\n").arg(WARRANTY);
    about += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    about += tr("This product uses Qt Libraries:\n");
    about += QString(" - %1\n").arg(qt_COPYRIGHT);
    about += QString(" - %1%2\n").arg(tr("QT Library License: "), qt_LICENSE);
    about += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    return about;
} //buildAboutText

void MainWindow::on_actionAbout_triggered()
{
    const QString version = buildAppVersionString();
    const QString compileDate = QDate::fromString(tr(__DATE__), "MMM dd yyyy").toString("yyyy");
    const QString releaseDate = tr(__DATE__);
    const QString compilerInfo = detectCompilerInfo();

    const QString aboutText = buildAboutText(version, compileDate, releaseDate, compilerInfo);

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(APP_NAME);
    aboutBox.setText(aboutText);
    aboutBox.exec();
} //on_actionAbout_triggered

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
} //on_actionAbout_Qt_triggered

bool MainWindow::copyResourceToFile(const QString &resourcePath, const QString &targetPath)
{
    QFile resourceFile(resourcePath);
    QFile targetFile(targetPath);

    if (targetFile.exists()) {
        if (!targetFile.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther)) {
            qWarning().nospace() << "[copyResourceToFile] Cannot change permissions on: " << targetPath << " | Reason: " << targetFile.errorString();
        }

        if (!targetFile.remove()) {
            qCritical().nospace() << "[copyResourceToFile] Failed to remove: " << targetPath << " | Reason: " << targetFile.errorString();
            return false;
        }

        qDebug().nospace() << "[copyResourceToFile] Existing file removed: " << targetPath;
    } else {
        qDebug().nospace() << "[copyResourceToFile] No existing file to replace: " << targetPath;
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
    const QString resourcePath = ":/resources" + fileName;
    const QString targetPath = qApp->applicationDirPath() + fileName;

    if (!copyResourceToFile(resourcePath, targetPath)) {
        qWarning() << "Failed to open resource file:" << targetPath;
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath));
} //openResourceFile

void MainWindow::on_actionQt_License_triggered()
{
    openResourceFile("/LGPLv3.pdf");
} //on_actionQt_License_triggered

void MainWindow::on_actionUser_Manual_triggered()
{
    openResourceFile("/UserManual.pdf");
} //on_actionUser_Manual_triggered

bool MainWindow::isValidIPv4Address(const QString &ip) const
{
    return !ip.contains(":") && !ip.startsWith("169.254");
} //isValidIPv4Address

void MainWindow::fillNetworkWidgets()
{
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
    ui->comboBoxSelectStyleSheet->setCurrentText(configSettings.stylesheetName);
} //updateUIWidgets

void MainWindow::setStyleSheet()
{
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
    return QStringLiteral("QTextEdit#textEditChat { "
                          "border-image: url(:/images/BackgroundImage10.png); "
                          "}");
} //chatBackgroundStyle

void MainWindow::setBackgroundImage()
{
    if (configSettings.b_displayBackgroundImage) {
        ui->textEditChat->setStyleSheet(chatBackgroundStyle());
    } else {
        ui->textEditChat->setStyleSheet(QString()); // reset to default
    }
} //setBackgroundImage

void MainWindow::displayMessages(const QList<Message> &messages)
{
    ui->textEditChat->clear();
    for (const Message &msg : messages) {
        m_formatter->appendMessage(ui->textEditChat, msg.user, msg.text, msg.timestamp, configSettings.b_isDarkThemed, msg.isSentByMe);
    }
} //displayMessages

int MainWindow::calculateClampedOffset(int requestedOffset, int totalMessages) const
{
    int maxOffset = qMax(0, totalMessages - messagesPerPage);
    return qBound(0, requestedOffset, maxOffset);
} //calculateClampedOffset

void MainWindow::loadPage(int offset)
{
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
    if (obj == ui->textEditChat->verticalScrollBar() && event->type() == QEvent::Type::Wheel && !isLoadingHistory) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        const int delta = wheelEvent->angleDelta().y();
        handleChatScroll(ui->textEditChat->verticalScrollBar(), delta < 0); // true if scrolling down
    }
    return QMainWindow::eventFilter(obj, event);
} //eventFilter

void MainWindow::handleChatScroll(QScrollBar *scrollBar, bool scrollingDown)
{
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
    if (lastScrollBarValue < 0) {
        lastScrollBarValue = value;
        return;
    }

    const bool scrollingDown = (value > lastScrollBarValue);
    lastScrollBarValue = value;

    handleChatScroll(ui->textEditChat->verticalScrollBar(), scrollingDown);
} //onChatScrollBarChanged

void MainWindow::initializeManagers()
{
    settingsManager = new SettingsManager(instanceID, QCoreApplication::applicationDirPath(), this);
    udpManager = new UdpChatSocketManager(this);
    m_formatter = new ChatFormatter(this);
} //initializeManagers

void MainWindow::initializeUi()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setTabEnabled(0, false); // Disable Chat tab
    ui->statusbar->addWidget(ui->labelStatus);
    ui->textEditChat->verticalScrollBar()->installEventFilter(this);
} //initializeUi

void MainWindow::connectSignals()
{
    connect(ui->textEditChat->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::onChatScrollBarChanged);

    connect(udpManager, &UdpChatSocketManager::messageReceived, this, [this](const QString &user, const QString &msg) {
        const QDateTime timestamp = QDateTime::currentDateTimeUtc();
        messageStore->insertMessage(user, msg, timestamp, false);
        m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, configSettings.b_isDarkThemed, false);
        ui->textEditChat->moveCursor(QTextCursor::End);

        if (isMinimized() || !isVisible() || !isActiveWindow()) {
            QApplication::alert(this, 3000);
            new ToastNotification(QString("%1: %2").arg(user, msg), this);
        }
    });
} //connectSignals

void MainWindow::loadInitialState()
{
    settingsManager->load(configSettings);
    restoreGeometry(settingsManager->loadGeometry());

    loadQStyleSheetFolder();
    setStyleSheet();
    setBackgroundImage();
    setAppWindowTitle();
    fillNetworkWidgets();
    updateUIWidgets();
} //loadInitialState

void MainWindow::initializeDatabase()
{
    // const QString dbPath = QCoreApplication::applicationDirPath() + "/chat_messages.db";
    const QString dbPath = QCoreApplication::applicationDirPath() + QString("/chat_messages_instance_%1.db").arg(instanceID);

    messageStore = new MessageStore(dbPath, instanceID, this);

    if (!messageStore->open()) {
        qCritical() << "Unable to load message database.";
        return;
    }

    const QList<Message> messages = messageStore->fetchLastMessages(20);

    ui->textEditChat->clear();
    for (const Message &msg : messages) {
        m_formatter->appendMessage(ui->textEditChat, msg.user, msg.text, msg.timestamp, configSettings.b_isDarkThemed, msg.isSentByMe);
    }

    currentOffset = messageStore->messageCount() - messages.size();
} //initializeDatabase

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isApplicationStarting = true;

    qDebug() << QSqlDatabase::drivers();

    instanceID = acquireInstanceId();
    initializeManagers();
    initializeUi();
    connectSignals();
    loadInitialState();
    initializeDatabase();

    userNameSaveDebounceTimer.setInterval(500);
    userNameSaveDebounceTimer.setSingleShot(true);
    connect(&userNameSaveDebounceTimer, &QTimer::timeout, this, [this]() { settingsManager->save(configSettings); });

    isApplicationStarting = false;
} //MainWindow

MainWindow::~MainWindow()
{
    if (settingsManager) {
        userNameSaveDebounceTimer.stop();
        settingsManager->save(configSettings);
        settingsManager->saveGeometry(saveGeometry());
    }

    releaseInstanceId(instanceID);

    delete ui;
    ui = nullptr;
} //~MainWindow

QByteArray MainWindow::buildRawUdpPayload(const QString &user, const QString &msg) const
{
    return QString("%1 - %2").arg(user, msg).toLocal8Bit();
} //buildRawUdpPayload

bool MainWindow::sendUdpMessage(const QByteArray &data, const QHostAddress &address, quint16 port)
{
    return udpManager->sendMessage(data, address, port) == data.size();
} //sendUdpMessage

void MainWindow::storeAndDisplaySentMessage(const QString &user, const QString &msg, const QDateTime &timestamp)
{
    messageStore->insertMessage(user, msg, timestamp, true);
    m_formatter->appendMessage(ui->textEditChat, user, msg, timestamp, configSettings.b_isDarkThemed, true);
} //storeAndDisplaySentMessage

void MainWindow::on_pushButtonSend_clicked()
{
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
    const QString text = ui->comboBoxLocalUDPNetwork->currentText().trimmed();
    return (text == "ANY") ? QHostAddress(QHostAddress::AnyIPv4) : QHostAddress(text);
} //parseLocalAddress

bool MainWindow::bindUdpSockets(const QHostAddress &local, const QHostAddress &remote, quint16 port)
{
    bool recvBound = udpManager->bindReceiveSocket(local, remote, port);
    bool sendBound = udpManager->bindSendSocket(local);
    return recvBound && sendBound;
} //bindUdpSockets

void MainWindow::updateUiOnConnectSuccess()
{
    ui->labelStatus->setText(tr("Connection established."));
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->frameUDPParameters->setEnabled(false);
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setCurrentIndex(0);
} //updateUiOnConnectSuccess

void MainWindow::on_pushButtonConnect_clicked()
{
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
    if (ui->pushButtonSend->isEnabled()) {
        on_pushButtonSend_clicked();
    }
} //on_lineEditChatText_returnPressed

void MainWindow::resetUiAfterDisconnect()
{
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false);
} //resetUiAfterDisconnect

void MainWindow::on_pushButtonDisconnect_clicked()
{
    if (udpManager)
        udpManager->closeSockets();

    resetUiAfterDisconnect();
    ui->labelStatus->setText(tr("Disconnected from network."));
} //on_pushButtonDisconnect_clicked

void MainWindow::on_checkBoxMulticast_clicked(bool checked)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_multicast, checked);

} //on_checkBoxMulticast_clicked

void MainWindow::on_checkBoxLoopback_clicked(bool checked)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_loopback, checked);

} //on_checkBoxLoopback_clicked

void MainWindow::on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPAddress, arg1);

} //on_comboBoxLocalUDPNetwork_currentTextChanged

void MainWindow::on_lineEditLocalUDPPort_textChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.localUDPPort, arg1.toUShort());

} //on_lineEditLocalUDPPort_textChanged

void MainWindow::on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPAddress, arg1);

} //on_lineEditRemoteUDPNetwork_textChanged

void MainWindow::on_lineEditRemoteUDPPort_textChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.remoteUDPPort, arg1.toUShort());

} //on_lineEditRemoteUDPPort_textChanged

void MainWindow::on_spinBoxTTL_valueChanged(int arg1)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.udpTTL, arg1);

} //on_spinBoxTTL_valueChanged

void MainWindow::on_comboBoxSelectStyleSheet_currentTextChanged(const QString &styleName)
{
    if (isApplicationStarting)
        return;

    const QString styleFile = QStyleSheetMap.value(styleName);
    settingsManager->update(configSettings.stylesheetName, styleFile);

    loadStyleSheet();
    settingsManager->save(configSettings);
} //on_comboBoxSelectStyleSheet_currentTextChanged

void MainWindow::populateStyleSheetMap(const QString &folderPath)
{
    QDirIterator it(folderPath, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        QStyleSheetMap.insert(fileInfo.baseName(), fileInfo.absoluteFilePath());
    }
} //populateStyleSheetMap

void MainWindow::populateStyleSheetComboBox()
{
    ui->comboBoxSelectStyleSheet->clear();
    ui->comboBoxSelectStyleSheet->addItem(""); // for 'none' or default
    ui->comboBoxSelectStyleSheet->addItems(QStyleSheetMap.keys());
} //populateStyleSheetComboBox

void MainWindow::on_checkBoxLoadStyleSheet_clicked(bool checked)
{
    if (isApplicationStarting) {
        return;
    }

    SettingsManager::update(configSettings.b_loadStyleSheet, checked);

} //on_checkBoxLoadStyleSheet_clicked

void MainWindow::loadQStyleSheetFolder()
{
    const QString styleFolder = QApplication::applicationDirPath() + "/../QStyleSheets";
    QStyleSheetMap.clear(); // clear existing entries

    populateStyleSheetMap(styleFolder);
    populateStyleSheetComboBox();
} //loadQStyleSheetFolder

QString MainWindow::readStyleSheetFile(const QString &path)
{
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

bool MainWindow::extractThemeFromStyleSheet(const QString &styleSheet) const
{
    QRegularExpression themeRegex(R"(/\*\s*theme:\s*(dark|light)\s*\*/)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = themeRegex.match(styleSheet);

    if (match.hasMatch()) {
        const QString theme = match.captured(1).toLower();
        return theme == "dark";
    }

    return false; // Default to light
} //extractThemeFromStyleSheet

void MainWindow::loadStyleSheet()
{
    const QString path = configSettings.stylesheetName;
    QString styleSheetContent = readStyleSheetFile(path);

    if (styleSheetContent.isEmpty()) {
        qApp->setStyleSheet("");
        QSignalBlocker block(ui->checkBoxLoadStyleSheet);
        ui->checkBoxLoadStyleSheet->setChecked(false);
        return;
    }

    configSettings.b_isDarkThemed = extractThemeFromStyleSheet(styleSheetContent);
    QTimer::singleShot(0, [styleSheetContent]() { qApp->setStyleSheet(styleSheetContent); });
} //loadStyleSheet

void MainWindow::on_checkBoxDisplayBackgroundImage_clicked(bool checked)
{
    if (isApplicationStarting)
        return;

    SettingsManager::update(configSettings.b_displayBackgroundImage, checked);
    settingsManager->save(configSettings);

    setBackgroundImage();
} //on_checkBoxDisplayBackgroundImage_clicked

void MainWindow::on_lineEditUserName_textChanged(const QString &newUserName)
{
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
    static uint msgNumber = 1;
    return QString("Test %1").arg(msgNumber++);
} //generateNextTestMessage

void MainWindow::on_pushButtonTestMsg_clicked()
{
    ui->lineEditChatText->setText(generateNextTestMessage());
} //on_pushButtonTestMsg_clicked

void MainWindow::on_pushButtonDeleteDatabase_clicked()
{
    if (QMessageBox::question(this, "Confirm Clear", "Are you sure you want to delete all chat messages?", QMessageBox::Yes | QMessageBox::No) ==
        QMessageBox::Yes) {
        if (messageStore->clearMessages()) {
            ui->textEditChat->clear();
            currentOffset = 0;
            ui->labelStatus->setText("Chat history cleared.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to clear chat history.");
        }
    }
} //on_pushButtonDeleteDatabase_clicked
