#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"

#include "chatformatter.h"
#include "udpchatsocketmanager.h"
#include "settingsmanager.h"
#include "toastnotification.h"
#include "messagestore.h"


#include <QScrollBar>
#include <QWheelEvent>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ChatFormatter *m_formatter = nullptr;
    UdpChatSocketManager *udpManager = nullptr;

    // Add member:
    MessageStore *messageStore = nullptr;
    int currentOffset = 0;
    const int messagesPerPage = 20;

    bool isLoadingHistory = false;
    int lastScrollBarValue = -1;

    /**
    * @brief instanceID
    */
    int instanceID = 0;
    /**
    * @brief isApplicationStarting
    */
    bool isApplicationStarting = false;
    /**
    * @brief configSettings
    */
    Settings configSettings;
    SettingsManager *settingsManager = nullptr;

    /**
    * @brief QStyleSheetMap
    */
    QMap<QString, QString> QStyleSheetMap;

    /**
     * @brief fillNetworkWidgets
     */
    void fillNetworkWidgets();
    /**
     * @brief updateUIWidgets
     */
    void updateUIWidgets();
    /**
     * @brief loadQStyleSheetFolder
     */
    void loadQStyleSheetFolder();
    /**
     * @brief loadStyleSheet
     */
    void loadStyleSheet();
    /**
     * @brief setStyleSheet
     */
    void setStyleSheet();
    /**
     * @brief setBackgroundImage
     */
    void setBackgroundImage();

    /**
    * @brief acquireInstanceId
    * @return
    */
    int acquireInstanceId();
    /**
    * @brief releaseInstanceId
    * @param instanceId
    */
    void releaseInstanceId(int instanceId);
    /**
    * @brief setAppWindowTitle
    */
    void setAppWindowTitle();

    /**
    * @brief openResourceFile
    * @param fileName
    */
    void openResourceFile(const QString &fileName);

    bool eventFilter(QObject *obj, QEvent *event) override;

    // void loadPreviousMessages();
    void loadPage(int offset);

    void handleChatScroll(QScrollBar *scrollBar, bool scrollingDown);
    QString buildVersionSuffix(const QString &version) const;
    QString getInstanceIdsFilePath() const;
    QString getInstanceIdsLockFilePath() const;
    QSet<int> loadInstanceIdsExcluding(int excludeId, const QString &filePath) const;
    void saveInstanceIds(const QSet<int> &ids, const QString &filePath) const;
    QString detectCompilerInfo() const;
    QString buildAppVersionString() const;
    QString buildAboutText(const QString &version, const QString &compileDate, const QString &releaseDate, const QString &compilerInfo) const;
    bool copyResourceToFile(const QString &resourcePath, const QString &targetPath);
    bool isValidIPv4Address(const QString &ip) const;
    QString chatBackgroundStyle() const;
    void displayMessages(const QList<Message> &messages);
    int calculateClampedOffset(int requestedOffset, int totalMessages) const;
    bool canScrollDown(int totalMessages) const;
    bool canScrollUp() const;
    void restoreScrollPositionAfterLoad(bool scrollToTop, bool edgeOfDatabase);
    void initializeManagers();
    void initializeUi();
    void initializeDatabase();
    void connectSignals();
    void loadInitialState();

    QByteArray buildRawUdpPayload(const QString &user, const QString &msg) const;
    bool sendUdpMessage(const QByteArray &data, const QHostAddress &address, quint16 port);
    void storeAndDisplaySentMessage(const QString &user, const QString &msg, const QDateTime &timestamp);
    QHostAddress parseLocalAddress() const;
    bool bindUdpSockets(const QHostAddress &local, const QHostAddress &remote, quint16 port);
    void updateUiOnConnectSuccess();
    void resetUiAfterDisconnect();
    void populateStyleSheetMap(const QString &folderPath);
    void populateStyleSheetComboBox();
    QString readStyleSheetFile(const QString &path);
    bool extractThemeFromStyleSheet(const QString &styleSheet) const;
    QString generateNextTestMessage();
private slots:
    /**
    * @brief on_actionAbout_triggered
    */
    void on_actionAbout_triggered();
    /**
    * @brief on_actionAbout_Qt_triggered
    */
    void on_actionAbout_Qt_triggered();
    /**
    * @brief on_actionQt_License_triggered
    */
    void on_actionQt_License_triggered();
    /**
    * @brief on_actionUser_Manual_triggered
    */
    void on_actionUser_Manual_triggered();
    /**
    * @brief on_pushButtonSend_clicked
    */
    void on_pushButtonSend_clicked();
    /**
    * @brief on_lineEditChatText_returnPressed
    */
    void on_lineEditChatText_returnPressed();
    /**
    * @brief on_pushButtonConnect_clicked
    */
    void on_pushButtonConnect_clicked();
    /**
    * @brief on_pushButtonDisconnect_clicked
    */
    void on_pushButtonDisconnect_clicked();
    /**
    * @brief on_checkBoxMulticast_clicked
    * @param checked
    */
    void on_checkBoxMulticast_clicked(bool checked);
    /**
    * @brief on_checkBoxLoopback_clicked
    * @param checked
    */
    void on_checkBoxLoopback_clicked(bool checked);
    /**
    * @brief on_spinBoxTTL_valueChanged
    * @param arg1
    */
    void on_spinBoxTTL_valueChanged(int arg1);
    /**
    * @brief on_comboBoxLocalUDPNetwork_currentTextChanged
    * @param arg1
    */
    void on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1);
    /**
    * @brief on_lineEditLocalUDPPort_textChanged
    * @param arg1
    */
    void on_lineEditLocalUDPPort_textChanged(const QString &arg1);
    /**
    * @brief on_lineEditRemoteUDPNetwork_textChanged
    * @param arg1
    */
    void on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1);
    /**
    * @brief on_lineEditRemoteUDPPort_textChanged
    * @param arg1
    */
    void on_lineEditRemoteUDPPort_textChanged(const QString &arg1);
    /**
    * @brief on_comboBoxSelectStyleSheet_currentTextChanged
    * @param arg1
    */
    void on_comboBoxSelectStyleSheet_currentTextChanged(const QString &arg1);
    /**
    * @brief on_checkBoxLoadStyleSheet_clicked
    * @param checked
    */
    void on_checkBoxLoadStyleSheet_clicked(bool checked);
    /**
    * @brief on_checkBoxDisplayBackgroundImage_clicked
    * @param checked
    */
    void on_checkBoxDisplayBackgroundImage_clicked(bool checked);
    /**
    * @brief on_lineEditUserName_textChanged
    * @param arg1
    */
    void on_lineEditUserName_textChanged(const QString &arg1);

    void on_pushButtonTestMsg_clicked();
    void onChatScrollBarChanged(int value);
    void on_pushButtonDeleteDatabase_clicked();
};

#endif // MAINWINDOW_H
