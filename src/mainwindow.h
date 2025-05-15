#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"

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

private slots:
    /**
    * @brief slotProcessPendingDatagrams
    */
    void slotProcessPendingDatagrams();
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

private:
    Ui::MainWindow *ui;
    QMap<QString, QColor> userColorMap;

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
    /**
    * @brief sendUDPSocket
    */
    QUdpSocket *sendUDPSocket = nullptr;
    /**
    * @brief recvUDPSocket
    */
    QUdpSocket *recvUDPSocket = nullptr;
    /**
    * @brief lastSentData
    */
    QByteArray lastSentData;
    /**
    * @brief QStyleSheetMap
    */
    QMap<QString, QString> QStyleSheetMap;
    /**
    * @brief bindRecvUDPSocket
    * @param localAddress
    * @param groupAddress
    * @param port
    * @return
    */
    bool bindRecvUDPSocket(const QHostAddress localAddress, const QHostAddress groupAddress, const quint16 port);
    /**
     * @brief bindSendUDPSocket
     * @param localAddress
     * @return
     */
    bool bindSendUDPSocket(const QHostAddress localAddress);
    /**
     * @brief fillNetworkWidgets
     */
    void fillNetworkWidgets();
    /**
     * @brief writeSettings
     */
    void writeSettings();
    /**
     * @brief readSettings
     */
    void readSettings();
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
     * @brief createSockets
     */
    void createSockets();
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
    * @brief updateSetting
    * @param settingRef
    * @param newValue
    */
    template<typename T>
    void updateSetting(T &settingRef, const T &newValue);
    /**
    * @brief openResourceFile
    * @param fileName
    */
    void openResourceFile(const QString fileName);
    void appendMessage(const QString &user, const QString &message, const QDateTime &timestamp, bool isSent);
    QColor generateColorForUser(const QString &user);
    bool isDarkTheme() const;
};

#endif // MAINWINDOW_H
