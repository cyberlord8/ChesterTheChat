#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::setAppWindowTitle()
{
    QString userName = ui->lineEditUserName->text();
    QString baseTitle = tr(APP_NAME) + QString(" - %1_%2").arg(QString::number(instanceID), userName);

    QString version = tr(VERSION);
    QString titleSuffix;

    if (version.contains("alpha", Qt::CaseInsensitive)) {
        QString buildDate = tr(BUILDDATE);
        int lastColonIndex = buildDate.lastIndexOf(':');
        if (lastColonIndex != -1) {
            buildDate = buildDate.left(lastColonIndex);
        }
        titleSuffix = QString(" - %1 - %2").arg(version, buildDate);
    } else if (version.contains("beta", Qt::CaseInsensitive)) {
        titleSuffix = QString(" - %1 - %2").arg(version, tr(RELEASEDATE));
    } else {
        titleSuffix = QString(" - %1").arg(tr(RELEASEDATE));
    }

    setWindowTitle(baseTitle + titleSuffix);
} //setAppWindowTitle

void MainWindow::releaseInstanceId(int instanceId)
{
    if (instanceId < 1)
        return;

    QString appDir = QCoreApplication::applicationDirPath();
    QString idsFilePath = appDir + "/instance_ids.txt";
    QString lockFilePath = appDir + "/instance_ids.lock";

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt for release");
        return;
    }

    QSet<int> usedIds;

    QFile idFile(idsFilePath);
    if (idFile.exists() && idFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&idFile);
        while (!in.atEnd()) {
            int id = in.readLine().toInt();
            if (id != instanceId) {
                usedIds.insert(id);
            }
        }
        idFile.close();
    }

    if (idFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&idFile);
        for (int id : std::as_const(usedIds)) {
            out << id << "\n";
        }
        idFile.close();
    }

    lock.unlock();
} //releaseInstanceId

int MainWindow::acquireInstanceId()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString idsFilePath = appDir + "/instance_ids.txt";
    QString lockFilePath = appDir + "/instance_ids.lock";

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt");
        return -1;
    }

    QSet<int> usedIds;

    QFile idFile(idsFilePath);
    if (idFile.exists() && idFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&idFile);
        while (!in.atEnd()) {
            usedIds.insert(in.readLine().toInt());
        }
        idFile.close();
    }

    int newId = 1;
    while (usedIds.contains(newId)) {
        ++newId;
    }

    usedIds.insert(newId);
    if (idFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&idFile);
        for (int id : std::as_const(usedIds)) {
            out << id << "\n";
        }
        idFile.close();
    }

    lock.unlock();
    return newId;
} //acquireInstanceId

void MainWindow::on_actionAbout_triggered()
{
    QString compilerInfo;
#ifdef ENV32
    compilerInfo = QString("Qt %1 MinGW (32-bit)").arg(qVersion());
#else
    compilerInfo = QString("Qt %1 MinGW (64-bit)").arg(qVersion());
#endif

    QString version = VERSION;
    if (version.contains("alpha", Qt::CaseInsensitive) || version.contains("bravo", Qt::CaseInsensitive)) {
        version += " " + tr(__TIME__); // Append compile time if in alpha/bravo
    }

    const QString compileDate = QDate::fromString(tr(__DATE__), "MMM dd yyyy").toString("yyyy");
    const QString releaseDate = tr(__DATE__);

    QString aboutText = tr(APP_LICENSE) + "\n";
    aboutText += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    aboutText += QString(" - %1 %2 %3\n").arg(COPYRIGHT, compileDate, COMPANY);
    aboutText += QString(" - %1\n").arg(RIGHTS);
    aboutText += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    aboutText += QString(" - %1%2\n").arg(tr("Version: "), version);
    aboutText += QString(" - %1%2\n").arg(tr("Release date: "), releaseDate);
    aboutText += QString(" - %1%2\n").arg(tr("Compiler: "), compilerInfo);
    aboutText += QString(" - %1%2\n").arg(tr("Running on: "), SYSTEMINFO);
    aboutText += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    aboutText += QString("%1\n").arg(WARRANTY);
    aboutText += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    aboutText += tr("This product uses Qt Libraries:\n");
    aboutText += QString(" - %1\n").arg(qt_COPYRIGHT);
    aboutText += QString(" - %1%2\n").arg(tr("QT Library License: "), qt_LICENSE);
    aboutText += tr("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(APP_NAME);
    aboutBox.setText(aboutText);
    aboutBox.exec();
} //on_actionAbout_triggered

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
} //on_actionAbout_Qt_triggered

void MainWindow::openResourceFile(const QString fileName)
{
    const QString resourcePath = ":/resources" + fileName;
    const QString targetPath = qApp->applicationDirPath() + fileName;

    QFile resourceFile(resourcePath);
    QFile existingFile(targetPath);

    if (existingFile.exists()) {
        if (!existingFile.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther)) {
            qDebug() << "Failed to change permissions on existing file:" << existingFile.errorString();
        }

        if (existingFile.remove()) {
            qDebug() << "Removed old file.";
        } else {
            qDebug() << "ERROR - Cannot remove file:" << existingFile.errorString();
        }
    } else {
        qDebug() << "No existing file found.";
    }

    if (!resourceFile.copy(targetPath)) {
        qDebug() << "Error copying file to:" << targetPath;
        qDebug() << "Reason:" << resourceFile.errorString();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath));
} //openResourceFile

void MainWindow::on_actionQt_License_triggered()
{
    const QString fileName = "/LGPLv3.pdf";
    openResourceFile(fileName);
} //on_actionQt_License_triggered

void MainWindow::on_actionUser_Manual_triggered()
{
    const QString fileName = "/UserManual.pdf";
    openResourceFile(fileName);
} //on_actionUser_Manual_triggered

void MainWindow::fillNetworkWidgets()
{
    ui->comboBoxLocalUDPNetwork->addItem("ANY");

    const QList<QHostAddress> ipAddresses = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : ipAddresses) {
        const QString ip = address.toString();

        // Skip IPv6
        if (ip.contains(":"))
            continue;

        // Skip link-local addresses (169.254.x.x)
        if (ip.startsWith("169.254"))
            continue;

        ui->comboBoxLocalUDPNetwork->addItem(ip);
    }
} //fillNetworkWidgets

void MainWindow::updateUIWidgets()
{
    ui->lineEditUserName->setText(configSettings.userName);
    ui->checkBoxLoopback->setChecked(configSettings.b_loopback);
    ui->checkBoxMulticast->setChecked(configSettings.b_multicast);
    ui->spinBoxTTL->setValue(configSettings.udpTTL);
    ui->comboBoxLocalUDPNetwork->setCurrentText(configSettings.localUDPAddress);
    ui->lineEditLocalUDPPort->setText(QString::number(configSettings.localUDPPort));
    ui->lineEditRemoteUDPNetwork->setText(configSettings.remoteUDPAddress);
    ui->lineEditRemoteUDPPort->setText(QString::number(configSettings.remoteUDPPort));

    ui->checkBoxDisplayBackgroundImage->setChecked(configSettings.b_displayBackgroundImage);

    ui->checkBoxLoadStyleSheet->setChecked(configSettings.b_loadStyleSheet);
    ui->comboBoxSelectStyleSheet->setCurrentText(configSettings.stylesheetName);
} //updateUIWidgets

void MainWindow::setStyleSheet()
{
    if (!configSettings.b_loadStyleSheet)
        return;

    const QString styleKey = QStyleSheetMap.key(configSettings.stylesheetName);

    if (!styleKey.isEmpty()) {
        ui->comboBoxSelectStyleSheet->setCurrentText(styleKey);
        loadStyleSheet();
    } else {
        qWarning() << "Style sheet not found in map for filename:" << configSettings.stylesheetName;
    }
} //setStyleSheet

void MainWindow::setBackgroundImage()
{
    if (configSettings.b_displayBackgroundImage) {
        const QString style = QStringLiteral("QTextEdit#textEditChat { "
                                             "border-image: url(:/images/BackgroundImage10.png); "
                                             "}");
        ui->textEditChat->setStyleSheet(style);
    } else {
        ui->textEditChat->setStyleSheet(QString());
    }
} //setBackgroundImage

void MainWindow::loadPage(int offset)
{
    if (isLoadingHistory)
        return; // prevent reentrancy

    isLoadingHistory = true;

    int total = messageStore->messageCount();
    if (total == 0) {
        isLoadingHistory = false;
        return;
    }

    int maxOffset = qMax(0, total - messagesPerPage);
    offset = qBound(0, offset, maxOffset);

    if (offset == currentOffset) {
        isLoadingHistory = false;
        return;
    }

    const QList<Message> messages = messageStore->fetchMessages(offset, messagesPerPage);

    ui->textEditChat->clear();
    for (const Message &msg : messages) {
        m_formatter->appendMessage(ui->textEditChat, msg.user, msg.text, msg.timestamp, configSettings.b_isDarkThemed, msg.isSentByMe);
    }

    currentOffset = offset;
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
}

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
} //

void MainWindow::onChatScrollBarChanged(int value)
{
    static int lastScrollValue = -1;
    if (lastScrollValue < 0) {
        lastScrollValue = value;
        return;
    }

    bool scrollingDown = value > lastScrollValue;
    lastScrollValue = value;

    handleChatScroll(ui->textEditChat->verticalScrollBar(), scrollingDown);
} //

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    instanceID = acquireInstanceId();
    settingsManager = new SettingsManager(instanceID, QCoreApplication::applicationDirPath(), this);
    settingsManager->load(configSettings);
    restoreGeometry(settingsManager->loadGeometry());

    udpManager = new UdpChatSocketManager(this);

    connect(ui->textEditChat->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::onChatScrollBarChanged);

    connect(udpManager, &UdpChatSocketManager::messageReceived, this, [this](const QString &user, const QString &msg) {
        messageStore->insertMessage(user, msg, QDateTime::currentDateTimeUtc(), false);
        m_formatter->appendMessage(ui->textEditChat, user, msg, QDateTime::currentDateTimeUtc(), configSettings.b_isDarkThemed, false);
        ui->textEditChat->moveCursor(QTextCursor::End);

        if (isMinimized() || !isVisible() || !isActiveWindow()) {
            QApplication::alert(this, 3000);                               // flash for 3 seconds
            new ToastNotification(QString("%1: %2").arg(user, msg), this); // auto-deletes on close
        }
    });

    m_formatter = new ChatFormatter(this);

    isApplicationStarting = true;

    loadQStyleSheetFolder();
    setStyleSheet();
    setBackgroundImage();
    setAppWindowTitle();

    fillNetworkWidgets();

    updateUIWidgets();

    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setTabEnabled(0, false); // Disable Chat tab
    ui->textEditChat->verticalScrollBar()->installEventFilter(this);
    ui->statusbar->addWidget(ui->labelStatus);

    QString dbPath = QCoreApplication::applicationDirPath() + "/chat_messages.db";
    messageStore = new MessageStore(dbPath, this);
    if (!messageStore->open()) {
        qCritical() << "Unable to load message database.";
    }

    const QList<Message> messages = messageStore->fetchLastMessages(20);

    ui->textEditChat->clear();
    for (const Message &msg : messages) {
        m_formatter->appendMessage(ui->textEditChat, msg.user, msg.text, msg.timestamp, configSettings.b_isDarkThemed, msg.isSentByMe);
    }

    currentOffset = messageStore->messageCount() - messages.size();

    isApplicationStarting = false;
} //MainWindow

MainWindow::~MainWindow()
{
#ifdef DEBUG_MODE
    qDebug() << Q_FUNC_INFO << "- Shutting down and saving settings.";
#endif

    settingsManager->save(configSettings);
    settingsManager->saveGeometry(saveGeometry());

    releaseInstanceId(instanceID);

    delete m_formatter;
    delete settingsManager;
    delete messageStore;
    delete ui;
    ui = nullptr;
} //MainWindow

void MainWindow::on_pushButtonSend_clicked()
{
    if (!udpManager)
        return;

    const QString messageText = ui->lineEditChatText->text().trimmed();
    if (messageText.isEmpty())
        return;

    const QString userName = ui->lineEditUserName->text().trimmed();
    const QString timestamp = QDateTime::currentDateTimeUtc().toString("hh:mm");

    QByteArray rawData = QString("%1 - %2").arg(userName, messageText).toLocal8Bit();

    QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());
    bool portOk = false;
    const quint16 port = ui->lineEditRemoteUDPPort->text().toUShort(&portOk);

    if (!portOk) {
        ui->labelStatus->setText("Invalid UDP port.");
        return;
    }

    const qint64 returnSize = udpManager->sendMessage(rawData, groupAddress, port);

    if (returnSize == rawData.size()) {
        messageStore->insertMessage(userName, messageText, QDateTime::currentDateTimeUtc(), true);
        m_formatter->appendMessage(ui->textEditChat, userName, messageText, QDateTime::currentDateTimeUtc(), configSettings.b_isDarkThemed, true);
    } else {
        const QString error = tr("%1 - ERROR writing to UDP socket: %2").arg(Q_FUNC_INFO, udpManager->lastError());
        ui->labelStatus->setText(error);
    }
} //on_pushButtonSend_clicked

void MainWindow::on_pushButtonConnect_clicked()
{
    if (!udpManager)
        return;

    bool portOk = false;
    const quint16 port = ui->lineEditLocalUDPPort->text().toUShort(&portOk);
    if (!portOk || port == 0) {
        ui->labelStatus->setText(tr("Invalid local UDP port."));
        return;
    }

    const QString localAddressText = ui->comboBoxLocalUDPNetwork->currentText();
    QHostAddress localAddress = (localAddressText == "ANY") ? QHostAddress(QHostAddress::AnyIPv4) : QHostAddress(localAddressText);

    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    bool recvBound = udpManager->bindReceiveSocket(localAddress, groupAddress, port);
    bool sendBound = udpManager->bindSendSocket(localAddress);

    if (recvBound && sendBound) {
        ui->labelStatus->setText(tr("Connection established."));
        ui->pushButtonConnect->setEnabled(false);
        ui->pushButtonDisconnect->setEnabled(true);
        ui->frameUDPParameters->setEnabled(false);
        ui->tabWidget->setCurrentIndex(0);
        ui->tabWidget->setTabEnabled(0, true);
    } else {
        ui->labelStatus->setText(tr("Failed to bind one or both sockets."));
    }
} //on_pushButtonConnect_clicked

void MainWindow::on_lineEditChatText_returnPressed()
{
    if (!ui->pushButtonSend->isEnabled())
        return;
    on_pushButtonSend_clicked();
} //on_lineEditChatText_returnPressed

void MainWindow::on_pushButtonDisconnect_clicked()
{
    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    if (udpManager)
        udpManager->closeSockets();

    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false); // Disable Chat tab

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

void MainWindow::on_comboBoxSelectStyleSheet_currentTextChanged(const QString &arg1)
{
    if (isApplicationStarting) {
        return;
    }

    settingsManager->update(configSettings.stylesheetName, QStyleSheetMap.value(arg1));
    loadStyleSheet();
    settingsManager->save(configSettings);
} //on_comboBoxSelectStyleSheet_currentTextChanged

void MainWindow::on_checkBoxLoadStyleSheet_clicked(bool checked)
{
    if (isApplicationStarting) {
        return;
    }

    SettingsManager::update(configSettings.b_loadStyleSheet, checked);

} //on_checkBoxLoadStyleSheet_clicked

void MainWindow::loadQStyleSheetFolder()
{
    QString QStyleSheetFolder = QApplication::applicationDirPath() + "/../QStyleSheets";
    QDirIterator fileIterator(QStyleSheetFolder, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);

    while (fileIterator.hasNext()) {
        QFile styleSheetFile(fileIterator.next());
        QFileInfo fileInfo(styleSheetFile);
        QStyleSheetMap.insert(fileInfo.baseName(), styleSheetFile.fileName());
    }

    ui->comboBoxSelectStyleSheet->clear();
    ui->comboBoxSelectStyleSheet->addItem("");
    ui->comboBoxSelectStyleSheet->addItems(QStyleSheetMap.keys());
} //loadQStyleSheetFolder

void MainWindow::loadStyleSheet()
{
    QFile styleSheetFile;
    styleSheetFile.setFileName(configSettings.stylesheetName);

    if (!styleSheetFile.exists()) {
        qApp->setStyleSheet("");
        ui->checkBoxLoadStyleSheet->setChecked(false);
        return;
    }

    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheetString = QLatin1String(styleSheetFile.readAll());
    styleSheetFile.close();
    configSettings.stylesheetName = styleSheetFile.fileName();

    // Detect theme from comment in stylesheet
    QRegularExpression themeRegex(R"(/\*\s*theme:\s*(dark|light)\s*\*/)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = themeRegex.match(styleSheetString);
    if (match.hasMatch()) {
        QString theme = match.captured(1).toLower();
        configSettings.b_isDarkThemed = (theme == "dark");
    } else {
        configSettings.b_isDarkThemed = false; // Default to light if not specified
    }

    QTimer::singleShot(0, [=] { qApp->setStyleSheet(styleSheetString); });
} //

void MainWindow::on_checkBoxDisplayBackgroundImage_clicked(bool checked)
{
    if (isApplicationStarting) {
        return;
    }

    SettingsManager::update(configSettings.b_displayBackgroundImage, checked);
    settingsManager->save(configSettings);

    if (checked) {
        const QString style = QStringLiteral("QTextEdit#textEditChat { "
                                             "border-image: url(:/images/BackgroundImage10.png); "
                                             "}");
        ui->textEditChat->setStyleSheet(style);
    } else {
        const QString style = QStringLiteral("QTextEdit#textEditChat { "
                                             "border-image: none; "
                                             "}");
        ui->textEditChat->setStyleSheet(style);
    }
} //on_checkBoxDisplayBackgroundImage_clicked

void MainWindow::on_lineEditUserName_textChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    configSettings.userName = arg1;
    setAppWindowTitle();
} //on_lineEditUserName_textChanged

void MainWindow::on_pushButtonTestMsg_clicked()
{
    static uint msgNumber = 1;
    ui->lineEditChatText->setText(QString("Test %1").arg(msgNumber++));
} //on_pushButtonTestMsg_clicked
