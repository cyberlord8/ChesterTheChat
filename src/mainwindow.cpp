#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::setAppWindowTitle()
{
    QString windowTitle = tr(APP_NAME);
    windowTitle += QString(" - %1_%2").arg(instanceID).arg(ui->lineEditUserName->text());

    const QString version = VERSION;

    if (version.contains("alpha", Qt::CaseInsensitive)) {
        // Trim seconds from build date (assumes format: YYYY-MM-DD HH:MM:SS)
        QString buildDateTrimmed = tr(BUILDDATE);
        int lastColon = buildDateTrimmed.lastIndexOf(':');
        if (lastColon != -1) {
            buildDateTrimmed = buildDateTrimmed.left(lastColon);
        }

        windowTitle += QString(" - %1 - %2").arg(tr(VERSION), buildDateTrimmed);
    }
    else if (version.contains("beta", Qt::CaseInsensitive)) {
        windowTitle += QString(" - %1 - %2").arg(tr(VERSION), tr(RELEASEDATE));
    }
    else {
        windowTitle += QString(" - %1").arg(tr(RELEASEDATE));
    }

    setWindowTitle(windowTitle);
}

void MainWindow::releaseInstanceId(int instanceId) {
    if (instanceId < 1) return;

    QString appDir = QCoreApplication::applicationDirPath();
    QString idsFilePath = appDir + "/instance_ids.txt";
    QString lockFilePath = appDir + "/instance_ids.lock";

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt for release");
        return;
    }

    QSet<int> usedIds;

    // Load current IDs
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

    // Save updated IDs
    if (idFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&idFile);
        for (int id : std::as_const(usedIds)) {
            out << id << "\n";
        }
        idFile.close();
    }

    lock.unlock();
}

int MainWindow::acquireInstanceId() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString idsFilePath = appDir + "/instance_ids.txt";
    QString lockFilePath = appDir + "/instance_ids.lock";

    QLockFile lock(lockFilePath);
    if (!lock.tryLock(5000)) {
        qWarning("Could not lock instance_ids.txt");
        return -1;
    }

    QSet<int> usedIds;

    // Load existing IDs
    QFile idFile(idsFilePath);
    if (idFile.exists() && idFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&idFile);
        while (!in.atEnd()) {
            usedIds.insert(in.readLine().toInt());
        }
        idFile.close();
    }

    // Find the smallest unused ID
    int newId = 1;
    while (usedIds.contains(newId)) {
        ++newId;
    }

    // Add the new ID and write back
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
}

void MainWindow::writeSettings()
{
#ifdef DEBUG_MODE
    qDebug() << Q_FUNC_INFO;
#endif

    const QString settingsFile = QCoreApplication::applicationDirPath()
                                 + QString("/instance_%1_settings.ini").arg(instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    // Application Group
    settings.beginGroup("Application");
    {
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        settings.setValue("userName", ui->lineEditUserName->text());
    }
    settings.endGroup();

    // StyleSheet Group
    settings.beginGroup("StyleSheet");
    {
        settings.setValue("styleSheetFilename", configSettings.styleSheetFilename);
        settings.setValue("loadStyleSheet", configSettings.b_loadStyleSheet);
        settings.setValue("displayBackgroundImage", configSettings.b_displayBackgroundImage);
    }
    settings.endGroup();

    // Network Group
    settings.beginGroup("Network");
    {
        settings.setValue("isMulticast", configSettings.isMulticast);
        settings.setValue("isLoopback", configSettings.isLoopback);
        settings.setValue("ttlValue", configSettings.ttlValue);
        settings.setValue("localUDPNetwork", configSettings.localUDPNetwork);
        settings.setValue("localUDPPort", configSettings.localUDPPort);
        settings.setValue("groupAddress", configSettings.groupAddress);
        settings.setValue("remoteUDPPort", configSettings.remoteUDPPort);
    }
    settings.endGroup();
}

void MainWindow::readSettings()
{
#ifdef DEBUG_MODE
    qDebug() << Q_FUNC_INFO;
#endif

    const QString settingsFile = QCoreApplication::applicationDirPath()
                                 + QString("/instance_%1_settings.ini").arg(instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    // Application Group
    settings.beginGroup("Application");
    {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
        ui->lineEditUserName->setText(settings.value("userName", "Chester").toString());
    }
    settings.endGroup();

    // StyleSheet Group
    settings.beginGroup("StyleSheet");
    {
        configSettings.styleSheetFilename         = settings.value("styleSheetFilename").toString();
        configSettings.b_loadStyleSheet           = settings.value("loadStyleSheet").toBool();
        configSettings.b_displayBackgroundImage   = settings.value("displayBackgroundImage").toBool();
    }
    settings.endGroup();

    // Network Group
    settings.beginGroup("Network");
    {
        configSettings.isMulticast     = settings.value("isMulticast", true).toBool();
        configSettings.isLoopback      = settings.value("isLoopback", true).toBool();
        configSettings.ttlValue        = settings.value("ttlValue", 5).toUInt();
        configSettings.localUDPNetwork = settings.value("localUDPNetwork", "ANY").toString();
        configSettings.localUDPPort    = settings.value("localUDPPort", "9999").toString();
        configSettings.groupAddress    = settings.value("groupAddress", "224.0.0.2").toString();
        configSettings.remoteUDPPort   = settings.value("remoteUDPPort", "9999").toString();
    }
    settings.endGroup();
}

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
        version += " " + tr(__TIME__);  // Append compile time if in alpha/bravo
    }

    const QString compileDate = QDate::fromString(tr(__DATE__), "MMM dd yyyy").toString("yyyy");
    const QString releaseDate = tr(__DATE__);

    // About message body
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

    // Show message box
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(APP_NAME);
    aboutBox.setText(aboutText);
    aboutBox.exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::openResourceFile(const QString fileName){
    const QString resourcePath = ":/resources" + fileName;
    const QString targetPath = qApp->applicationDirPath() + fileName;

    QFile resourceFile(resourcePath);
    QFile existingFile(targetPath);

    // Remove old file if it exists
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

    // Copy the file from resources
    if (!resourceFile.copy(targetPath)) {
        qDebug() << "Error copying file to:" << targetPath;
        qDebug() << "Reason:" << resourceFile.errorString();
    }

    // Open the copied PDF
    QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath));
}

void MainWindow::on_actionQt_License_triggered()
{
    const QString fileName = "/LGPLv3.pdf";
    openResourceFile(fileName);
}

void MainWindow::on_actionUser_Manual_triggered() {
    const QString fileName = "/UserManual.pdf";
    openResourceFile(fileName);
}

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
}

void MainWindow::updateUIWidgets()
{
    // Networking settings
    ui->checkBoxLoopback->setChecked(configSettings.isLoopback);
    ui->checkBoxMulticast->setChecked(configSettings.isMulticast);
    ui->spinBoxTTL->setValue(configSettings.ttlValue);
    ui->comboBoxLocalUDPNetwork->setCurrentText(configSettings.localUDPNetwork);
    ui->lineEditLocalUDPPort->setText(configSettings.localUDPPort);
    ui->lineEditRemoteUDPNetwork->setText(configSettings.groupAddress);
    ui->lineEditRemoteUDPPort->setText(configSettings.remoteUDPPort);

    // UI appearance
    ui->checkBoxDisplayBackgroundImage->setChecked(configSettings.b_displayBackgroundImage);

    // Style sheet settings
    ui->checkBoxLoadStyleSheet->setChecked(configSettings.b_loadStyleSheet);
    ui->comboBoxSelectStyleSheet->setCurrentText(configSettings.styleSheetFilename);
}

void MainWindow::setStyleSheet()
{
    if (!configSettings.b_loadStyleSheet)
        return;

    const QString styleKey = QStyleSheetMap.key(configSettings.styleSheetFilename);

    if (!styleKey.isEmpty()) {
        ui->comboBoxSelectStyleSheet->setCurrentText(styleKey);
        loadStyleSheet();
    } else {
        qWarning() << "Style sheet not found in map for filename:"
                   << configSettings.styleSheetFilename;
    }
}

void MainWindow::setBackgroundImage()
{
    if (configSettings.b_displayBackgroundImage) {
        const QString style = QStringLiteral(
            "QPlainTextEdit { "
            "background-image: url(:/images/BackgroundImage10.png); "
            "background-repeat: no-repeat; "
            "background-position: center; "
            "}"
            );
        ui->plainTextEdit->setStyleSheet(style);
    } else {
        // Clear background if option is disabled
        ui->plainTextEdit->setStyleSheet(QString());
    }
}

void MainWindow::createSockets()
{
    // Avoid accidental re-creation without cleanup
    if (sendUDPSocket || recvUDPSocket) {
        qWarning() << "UDP sockets already created — skipping createSockets().";
        return;
    }

    sendUDPSocket = new QUdpSocket(this);
    recvUDPSocket = new QUdpSocket(this);

#ifdef DEBUG_MODE
    qDebug() << "UDP sockets created.";
#endif
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Prevent event handlers from reacting during initial setup
    isApplicationStarting = true;

    // Load configuration
    instanceID = acquireInstanceId();
    readSettings();

    // Load styles and appearance
    loadQStyleSheetFolder();
    setStyleSheet();
    setBackgroundImage();
    setAppWindowTitle();

    // Initialize networking
    createSockets();
    fillNetworkWidgets();

    // Sync config to UI
    updateUIWidgets();

    // UI startup state
    ui->tabWidget->setCurrentIndex(1);

    isApplicationStarting = false;
}

MainWindow::~MainWindow()
{
#ifdef DEBUG_MODE
    qDebug() << Q_FUNC_INFO << "- Shutting down and saving settings.";
#endif

    writeSettings();
    releaseInstanceId(instanceID);

    delete ui;
    ui = nullptr;
}

void MainWindow::on_pushButtonSend_clicked()
{
    const QString messageText = ui->lineEditChatText->text().trimmed();
    if (messageText.isEmpty())
        return;

    const QString userName = ui->lineEditUserName->text().trimmed();
    const QString timestamp = QDateTime::currentDateTimeUtc().toString("dd MMM - hh:mm:ss");
    const QString displayMessage = QString("Sent: %1 - %2 - %3").arg(timestamp, userName, messageText);

    QByteArray rawData = QString("%1 - %2").arg(userName, messageText).toLocal8Bit();

    QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());
    bool portOk = false;
    const quint16 port = ui->lineEditRemoteUDPPort->text().toUShort(&portOk);

    if (!portOk) {
        ui->plainTextEdit->appendPlainText(tr("Invalid UDP port."));
        return;
    }

    const qint64 returnSize = sendUDPSocket->writeDatagram(rawData, groupAddress, port);

    if (returnSize == rawData.size()) {
        lastSentData = rawData;
        ui->plainTextEdit->appendPlainText(displayMessage);
        ui->lineEditChatText->clear();
    } else {
        const QString error = tr("%1 - ERROR writing to UDP socket: %2")
        .arg(Q_FUNC_INFO, sendUDPSocket->errorString());
        ui->plainTextEdit->appendPlainText(error);
    }
}

bool MainWindow::bindRecvUDPSocket(const QHostAddress localAddress,
                                   const QHostAddress groupAddress,
                                   const quint16 port)
{
    // Attempt to bind socket
    if (!recvUDPSocket->bind(localAddress, port, QUdpSocket::ReuseAddressHint)) {
        const QString error = tr("%1 - ERROR: Failed to bind UDP socket on %2:%3 - %4")
        .arg(Q_FUNC_INFO, localAddress.toString(), QString::number(port), recvUDPSocket->errorString());
        ui->plainTextEdit->appendPlainText(error);
        return false;
    }

    // Configure multicast options if enabled
    if (ui->checkBoxMulticast->isChecked()) {
        const bool enableLoopback = ui->checkBoxLoopback->isChecked();
        recvUDPSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, enableLoopback);

        if (!recvUDPSocket->joinMulticastGroup(groupAddress)) {
            ui->plainTextEdit->appendPlainText(tr("Warning: Failed to join multicast group: ") + groupAddress.toString());
        }
    }

    // Connect readyRead signal only once (optional safety)
    static bool signalConnected = false;
    if (!signalConnected) {
        connect(recvUDPSocket, &QUdpSocket::readyRead,
                this, &MainWindow::slotProcessPendingDatagrams);
        signalConnected = true;
    }

    ui->plainTextEdit->appendPlainText("Recv socket bound... OK.");
    return true;
}

bool MainWindow::bindSendUDPSocket(const QHostAddress localAddress)
{
    // Attempt to bind to the specified local address (port 0 = ephemeral)
    if (!sendUDPSocket->bind(localAddress, 0, QUdpSocket::ReuseAddressHint)) {
        const QString error = tr("%1 - ERROR: Failed to bind UDP send socket on %2 - %3")
        .arg(Q_FUNC_INFO, localAddress.toString(), sendUDPSocket->errorString());
        ui->plainTextEdit->appendPlainText(error);
        return false;
    }

    // Configure multicast options if enabled
    if (ui->checkBoxMulticast->isChecked()) {
        const bool enableLoopback = ui->checkBoxLoopback->isChecked();
        sendUDPSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, enableLoopback);

        const int ttl = ui->spinBoxTTL->value();
        sendUDPSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, ttl);
    }

    ui->plainTextEdit->appendPlainText("Send socket bound... OK.");
    return true;
}

void MainWindow::on_pushButtonConnect_clicked()
{
    // Parse local UDP port
    bool portOk = false;
    const quint16 port = ui->lineEditLocalUDPPort->text().toUShort(&portOk);
    if (!portOk || port == 0) {
        ui->plainTextEdit->appendPlainText(tr("Invalid local UDP port."));
        return;
    }

    // Determine local bind address
    const QString localAddressText = ui->comboBoxLocalUDPNetwork->currentText();
    QHostAddress localAddress = (localAddressText == "ANY")
                                    ? QHostAddress(QHostAddress::AnyIPv4)
                                    : QHostAddress(localAddressText);

    // Parse group (remote) address
    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    // Attempt to bind sockets
    bool recvBound = bindRecvUDPSocket(localAddress, groupAddress, port);
    bool sendBound = bindSendUDPSocket(localAddress);

    if (recvBound && sendBound) {
        ui->plainTextEdit->appendPlainText(tr("Connection established."));
        ui->pushButtonConnect->setEnabled(false);
        ui->pushButtonDisconnect->setEnabled(true);
        ui->frameUDPParameters->setEnabled(false);
        ui->tabWidget->setCurrentIndex(0);
    } else {
        ui->plainTextEdit->appendPlainText(tr("Failed to bind one or both sockets."));
    }
}

void MainWindow::slotProcessPendingDatagrams()
{
    while (recvUDPSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(qsizetype(recvUDPSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 senderPort = 0;

        recvUDPSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // Ignore echo of last sent message (self-check)
        if (datagram == lastSentData) {
            lastSentData.clear();  // Reset for future comparison
            continue;              // Skip this datagram, but continue processing others
        }

        const QString timestamp = QDateTime::currentDateTimeUtc().toString("dd MMM - hh:mm:ss");
        const QString processedText = QString("Rcvd: %1 - %2").arg(timestamp, QString::fromUtf8(datagram));

        ui->plainTextEdit->appendPlainText(processedText);
    }
}

void MainWindow::on_lineEditChatText_returnPressed()
{
    if (!ui->pushButtonSend->isEnabled()) return;
    on_pushButtonSend_clicked();
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    const QHostAddress groupAddress(ui->lineEditRemoteUDPNetwork->text().trimmed());

    // Leave multicast group (if bound and active)
    if (recvUDPSocket && recvUDPSocket->state() == QAbstractSocket::BoundState) {
        if (ui->checkBoxMulticast->isChecked()) {
            if (!recvUDPSocket->leaveMulticastGroup(groupAddress)) {
                ui->plainTextEdit->appendPlainText(tr("Warning: Failed to leave multicast group: ") + groupAddress.toString());
            }
        }
        recvUDPSocket->close();
    }

    if (sendUDPSocket && sendUDPSocket->state() == QAbstractSocket::BoundState) {
        sendUDPSocket->close();
    }

    // Update UI state
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->frameUDPParameters->setEnabled(true);

    ui->plainTextEdit->appendPlainText(tr("Disconnected from network."));
}

void MainWindow::on_checkBoxMulticast_clicked(bool checked)
{
    if (isApplicationStarting)
        return;

    updateSetting(configSettings.isMulticast, checked);

}

void MainWindow::on_checkBoxLoopback_clicked(bool checked)
{
    if (isApplicationStarting)
        return;

    updateSetting(configSettings.isLoopback, checked);

}

void MainWindow::on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1)
{
    if (isApplicationStarting)
        return;

    updateSetting(configSettings.localUDPNetwork, arg1);

}

void MainWindow::on_lineEditLocalUDPPort_textChanged(const QString &arg1)
{
    if(isApplicationStarting)
        return;

    updateSetting(configSettings.localUDPPort, arg1);

}

void MainWindow::on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1)
{
    if(isApplicationStarting)
        return;

    updateSetting(configSettings.groupAddress, arg1);

}

void MainWindow::on_lineEditRemoteUDPPort_textChanged(const QString &arg1)
{
    if(isApplicationStarting)
        return;

    updateSetting(configSettings.remoteUDPPort, arg1);

}

void MainWindow::on_spinBoxTTL_valueChanged(int arg1)
{
    if(isApplicationStarting)
        return;

    updateSetting(configSettings.ttlValue, static_cast<quint8>(arg1));

}

void MainWindow::on_comboBoxSelectStyleSheet_currentTextChanged(const QString &arg1)
{
    if(isApplicationStarting){
        return;
    }

    configSettings.styleSheetFilename = QStyleSheetMap.value(arg1);

    loadStyleSheet();
    writeSettings();
}

void MainWindow::on_checkBoxLoadStyleSheet_clicked(bool checked)
{
    if(isApplicationStarting){
        return;
    }

    updateSetting(configSettings.b_loadStyleSheet, checked);

}

void MainWindow::loadQStyleSheetFolder()
{
    QString QStyleSheetFolder = QApplication::applicationDirPath()+"./QStyleSheets";
    QDirIterator fileIterator(QStyleSheetFolder, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);

    while(fileIterator.hasNext()) {
        QFile styleSheetFile(fileIterator.next());
        QFileInfo fileInfo(styleSheetFile);
        QStyleSheetMap.insert(fileInfo.baseName(),styleSheetFile.fileName());
    }

    ui->comboBoxSelectStyleSheet->clear();
    ui->comboBoxSelectStyleSheet->addItem("");
    ui->comboBoxSelectStyleSheet->addItems(QStyleSheetMap.keys());
}

void MainWindow::loadStyleSheet()
{
    QFile styleSheetFile;
    styleSheetFile.setFileName(configSettings.styleSheetFilename);
    if(!styleSheetFile.exists()){
        qApp->setStyleSheet("");
        ui->checkBoxLoadStyleSheet->setChecked(false);
        return;//then return
    }
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheetString = QLatin1String(styleSheetFile.readAll());
    styleSheetFile.close();
    configSettings.styleSheetFilename = styleSheetFile.fileName();
    QTimer::singleShot(0, [=] {
        qApp->setStyleSheet(styleSheetString);
    });//
}

void MainWindow::on_checkBoxDisplayBackgroundImage_clicked(bool checked)
{
    if(isApplicationStarting){
        return;
    }

    updateSetting(configSettings.b_displayBackgroundImage, checked);

    if(checked){
        ui->plainTextEdit->setStyleSheet("QPlainTextEdit { "
                                         "background-image: url(:/images/BackgroundImage10.png); "
                                         "background-repeat: no-repeat; "
                                         "background-position: center; "
                                         " }");

    }
    else{
        ui->plainTextEdit->setStyleSheet("QPlainTextEdit { "
                                         "background-image: url(); "
                                         "background-repeat: no-repeat; "
                                         "background-position: center; "
                                         " }");
    }
}

void MainWindow::on_lineEditUserName_textChanged(const QString &arg1) {
    if(isApplicationStarting)
        return;

    setAppWindowTitle();
}

template<typename T>
void MainWindow::updateSetting(T &settingRef, const T &newValue)
{
    if (settingRef != newValue) {
        settingRef = newValue;
        writeSettings();
    }
}

void MainWindow::on_pushButtonCreateTestMsg_clicked()
{
    static uint increment = 1;
    ui->lineEditChatText->setText(QStringLiteral("Test %1").arg(increment++));
}
