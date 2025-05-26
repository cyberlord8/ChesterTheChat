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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"

#include "chatformatter.h"
#include "messagestore.h"
#include "settingsmanager.h"
#include "toastnotification.h"
#include "udpchatsocketmanager.h"

#include "DemoChatSimulator.h"
#include "StyleRotator.h"

#include <QScrollBar>
#include <QWheelEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define NUM_MSGS_PER_PAGE 20

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool isDemoRunning = false;
    QString realStyleSheetName = "";

    /**
 * @brief Simulates a timed, themed chat between Wonderland characters.
 *
 * This object runs a scripted sequence of messages using characters like
 * Alice, the Mad Hatter, and the Cheshire Cat. Messages appear in real-time
 * intervals, simulating a natural conversation.
 *
 * Used to demonstrate how the chat interface handles formatting, message stacking,
 * and timestamping in a controlled environment.
 */
    DemoChatSimulator *demoSimulator = nullptr;

    /**
 * @brief Automatically rotates through available Chester stylesheets.
 *
 * This object cycles through all registered `.qss` themes via the stylesheet
 * combo box. It's used to demonstrate the appearance of different UI themes
 * during demo mode, highlighting light and dark theme support.
 */
    StyleRotator *styleRotator = nullptr;

    /**
 * @brief Debounce timer used to delay saving the user name after edits.
 *
 * Prevents frequent disk writes or settings updates while the user is still typing.
 * Starts/restarts on each text change and triggers a save only after a short delay.
 */
    QTimer userNameSaveDebounceTimer;

    /**
 * @brief Index offset of the first message currently visible in the chat window.
 *
 * Used for pagination and redrawing the current message block, especially after
 * theme changes or scroll-triggered refreshes.
 */
    int visibleOffset;

    /**
 * @brief Number of messages currently visible in the chat window.
 *
 * Helps redraw only the displayed message block without reloading the entire history.
 * Typically set after fetching a paged result from the message store.
 */
    qsizetype visibleLimit;


    /** @name Chat Message State
     *  Tracks chat rendering, history paging, and scroll position.
     */
    ///@{
    /**
     * @brief Formats and inserts messages into the chat display.
     */
    ChatFormatter *m_formatter = nullptr;

    /**
     * @brief Manages storage and retrieval of chat messages from the local database.
     */
    MessageStore *messageStore = nullptr;

    /**
     * @brief Current offset into the message list for pagination.
     */
    int currentOffset = 0;

    /**
     * @brief Number of messages to load per page during scroll events.
     */
    const int messagesPerPage = NUM_MSGS_PER_PAGE;

    /**
     * @brief True if messages are currently being loaded, used to prevent reentrant loads.
     */
    bool isLoadingHistory = false;

    /**
     * @brief Tracks the last known scrollbar value to determine scroll direction.
     */
    int lastScrollBarValue = -1;
    ///@}

    /** @name UDP Communication
     *  Handles networking via UDP sockets.
     */
    ///@{
    UdpChatSocketManager *udpManager = nullptr;
    ///@}

    /** @name Application Configuration
     *  Tracks application settings, stylesheet options, and instance identity.
     */
    ///@{
    /**
     * @brief Holds the current user and UI configuration settings.
     */
    Settings configSettings;

    /**
     * @brief Handles loading and saving of settings to disk per instance.
     */
    SettingsManager *settingsManager = nullptr;

    /**
     * @brief Unique identifier for this instance of the application.
     * Used to isolate settings and avoid conflicts between multiple instances.
     */
    int instanceID = 0;

    /**
     * @brief Indicates whether the application is still initializing.
     * Used to prevent unintended slot reactions during startup.
     */
    bool isApplicationStarting = false;

    /**
     * @brief Maps stylesheet display names to their corresponding file paths.
     */
    QMap<QString, QString> QStyleSheetMap;
    ///@}

    /** @name Initialization and Lifecycle
     *  One-time setup operations executed during application startup.
     */
    ///@{
    /**
     * @brief Initializes core application managers (settings, networking, etc.).
     */
    void initializeManagers();

    /**
     * @brief Sets up and configures the user interface elements.
     */
    void initializeUi();

    /**
     * @brief Opens the message database and prepares the schema if needed.
     */
    void initializeDatabase();

    /**
     * @brief Connects signals and slots between UI and internal components.
     */
    void connectSignals();

    /**
     * @brief Loads initial application state such as settings and message history.
     */
    void loadInitialState();
    ///@}

    /** @name UI Setup and Sync
     *  Methods for initializing and updating user interface elements.
     */
    ///@{
    /**
     * @brief Populates the local network interface combo box with IPv4 addresses.
     *
     * Adds all non-link-local, non-loopback IPv4 addresses from the system's network interfaces
     * to the UI element used for selecting a local UDP interface.
     */
    void fillNetworkWidgets();

    /**
     * @brief Updates the UI controls to reflect the current application settings.
     *
     * Applies values from the loaded `Settings` object to relevant widgets including
     * username, network fields, and UI preference checkboxes.
     */
    void updateUIWidgets();
    ///@}

    /** @name Chat Paging and Scroll Handling
     *  Handles chat history paging and scrollbar-driven navigation.
     */
    ///@{
    /**
     * @brief Loads a page of messages starting at the given offset.
     * @param offset The starting index into the message list.
     */
    void loadPage(int offset);

    /**
     * @brief Responds to user scroll actions and loads the appropriate message page.
     * @param scrollBar The vertical scrollbar of the chat view.
     * @param scrollingDown True if the user is scrolling downward; otherwise false.
     */
    void handleChatScroll(QScrollBar *scrollBar, bool scrollingDown);

    /**
     * @brief Event filter for intercepting and handling custom scroll behavior.
     * @param obj The object that received the event.
     * @param event The event to process.
     * @return True if the event was handled internally; otherwise false.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Renders a list of messages into the chat view.
     * @param messages The list of Message objects to display.
     */
    void displayMessages(const QList<Message> &messages);

    /**
     * @brief Ensures the requested offset is clamped within valid message bounds.
     * @param requestedOffset The requested offset.
     * @param totalMessages The total number of available messages.
     * @return A clamped offset within valid range.
     */
    int calculateClampedOffset(int requestedOffset, int totalMessages) const;

    /** @name Instance ID Management
     *  Handles reservation, release, and persistence of application instance IDs.
     */
    ///@{
    /**
     * @brief Acquires a unique instance ID for the running application.
     * @return An integer representing a unique instance ID.
     */
    int acquireInstanceId();

    /**
     * @brief Releases a previously acquired instance ID, making it reusable.
     * @param instanceId The ID to release.
     */
    void releaseInstanceId(int instanceId);

    /**
     * @brief Retrieves the full file path where instance IDs are stored.
     * @return The path to instance_ids.txt.
     */
    QString getInstanceIdsFilePath() const;

    /**
     * @brief Retrieves the full file path to the instance ID lock file.
     * @return The path to instance_ids.lock.
     */
    QString getInstanceIdsLockFilePath() const;

    /**
     * @brief Loads a set of instance IDs from a file, excluding a specific ID.
     * @param excludeId The ID to omit from the result.
     * @param filePath Path to the instance ID file.
     * @return A set of all stored instance IDs except the excluded one.
     */
    QSet<int> loadInstanceIdsExcluding(int excludeId, const QString &filePath) const;

    /**
     * @brief Saves a set of instance IDs to a file, overwriting its contents.
     * @param ids The set of instance IDs to write.
     * @param filePath The destination file path.
     */
    void saveInstanceIds(const QSet<int> &ids, const QString &filePath) const;
    ///@}

    /** @name UI Style and Appearance
     *  Handles dynamic stylesheet loading and customization of the chat interface.
     */
    ///@{
    /**
     * @brief Applies the currently selected stylesheet if enabled in settings.
     */
    void setStyleSheet();

    /**
     * @brief Loads the stylesheet from file, applies it, and detects theme mode.
     */
    void loadStyleSheet();

    /**
     * @brief Sets or clears the background image of the chat window.
     * Applied only if enabled in settings.
     */
    void setBackgroundImage();

    /**
     * @brief Loads available .qss files from the stylesheet directory into an internal map.
     */
    void loadQStyleSheetFolder();

    /**
     * @brief Populates the internal stylesheet map from a specified directory.
     * @param folderPath The directory containing .qss files.
     */
    void populateStyleSheetMap(const QString &folderPath);

    /**
     * @brief Updates the UI combo box with the available stylesheets.
     */
    void populateStyleSheetComboBox();

    /**
     * @brief Reads the content of a stylesheet file.
     * @param path The full file path to the .qss file.
     * @return The stylesheet contents as a QString, or an empty string if failed.
     */
    QString readStyleSheetFile(const QString &path);

    /**
     * @brief Returns the current chat background style string based on user settings.
     * @return A QString with valid CSS style for QTextEdit background.
     */
    QString chatBackgroundStyle() const;
    ///@}

    /** @name Application Info and Metadata
     *  Functions for building and displaying version, build, and title information.
     */
    ///@{
    /**
     * @brief Sets the main application window title with version, instance, and user info.
     */
    void setAppWindowTitle();

    /**
     * @brief Builds a suffix string for the window title based on version and release stage.
     * @param version The application version string (e.g., "1.2.0-alpha").
     * @return A formatted version suffix (e.g., " - alpha - build date").
     */
    QString buildVersionSuffix(const QString &version) const;

    /**
     * @brief Constructs the full application version string including compile-time data.
     * @return The full version string with release or build context.
     */
    QString buildAppVersionString() const;

    /**
     * @brief Detects the Qt and compiler version used to build the application.
     * @return A descriptive string with compiler architecture and Qt version.
     */
    QString detectCompilerInfo() const;

    /**
     * @brief Constructs the full "About" dialog text block with license, version, and system info.
     * @param version The app version.
     * @param compileDate The compile-time date string.
     * @param releaseDate The configured release date string.
     * @param compilerInfo The compiler and platform used to build.
     * @return A formatted multi-line string used in the About dialog.
     */
    QString buildAboutText(const QString &version, const QString &compileDate, const QString &releaseDate, const QString &compilerInfo) const;
    ///@}

    /** @name UDP Messaging
     *  Handles chat message encoding, sending, and socket binding for UDP communication.
     */
    ///@{
    /**
     * @brief Constructs a raw UDP payload in the format "user - message".
     * @param user The sender's username.
     * @param msg The message content.
     * @return A QByteArray representing the encoded UDP payload.
     */
    QByteArray buildRawUdpPayload(const QString &user, const QString &msg) const;

    /**
     * @brief Sends a UDP datagram to the specified address and port.
     * @param data The message payload to send.
     * @param address The target IP address.
     * @param port The target UDP port.
     * @return True if the message was successfully sent, false otherwise.
     */
    bool sendUdpMessage(const QByteArray &data, const QHostAddress &address, quint16 port);

    /**
     * @brief Stores a sent message in the database and appends it to the chat view.
     * @param user The local user's name.
     * @param msg The sent message content.
     * @param timestamp The UTC timestamp for the message.
     */
    void storeAndDisplaySentMessage(const QString &user, const QString &msg, const QDateTime &timestamp);

    /**
     * @brief Parses and returns the local IP address selected in the UI.
     * @return A QHostAddress representing the selected local address or QHostAddress::AnyIPv4.
     */
    QHostAddress parseLocalAddress() const;

    /**
     * @brief Binds both the send and receive sockets using provided addresses and port.
     * @param local The local network address to bind.
     * @param remote The remote/multicast group address to join (if applicable).
     * @param port The UDP port number.
     * @return True if both sockets were successfully bound; false otherwise.
     */
    bool bindUdpSockets(const QHostAddress &local, const QHostAddress &remote, quint16 port);

    /**
     * @brief Updates the UI after a successful UDP connection is established.
     */
    void updateUiOnConnectSuccess();

    /**
     * @brief Resets the UI elements after disconnecting from the UDP session.
     */
    void resetUiAfterDisconnect();

    /**
     * @brief Validates whether the given string represents a valid IPv4 address.
     * @param ip The IP address string to validate.
     * @return True if the address is a valid IPv4 address; false otherwise.
     */
    bool isValidIPv4Address(const QString &ip) const;

    /**
     * @brief Generates the next sequential test message for debugging.
     * @return A test message string like "Test 1", "Test 2", etc.
     */
    QString generateNextTestMessage();
    ///@}

    /** @name Resource Utilities
     *  Helpers for extracting and opening embedded Qt resource files.
     */
    ///@{
    /**
     * @brief Copies a resource file to the application directory and opens it using the default system handler.
     * @param fileName The relative file path within the Qt resource system (e.g., "/UserManual.pdf").
     */
    void openResourceFile(const QString &fileName);

    /**
     * @brief Copies a file from the Qt resource system to the specified target path.
     * If the target file exists, it will be replaced.
     * @param resourcePath The full Qt resource path (e.g., ":/resources/help.pdf").
     * @param targetPath The destination path on the local file system.
     * @return True if the file was copied successfully; false otherwise.
     */
    bool copyResourceToFile(const QString &resourcePath, const QString &targetPath);
    ///@}

    /**
 * @brief Redraws the currently visible messages in the chat window.
 *
 * Refetches and redisplays the subset of messages currently in view,
 * using the stored visibleOffset and visibleLimit values. Typically
 * called after a theme change to ensure message styling reflects the
 * current stylesheet (e.g., for light/dark mode contrast).
 */
    void redrawCurrentMessages();

    /**
 * @brief Updates UI and internal state to reflect demo mode activation.
 *
 * This function is called after the demo simulator has been successfully started.
 * It disables network interaction, enables the demo tab, updates button labels,
 * and begins automatic style rotation.
 */
    void startDemoModeUiSetup();

    /**
 * @brief Reverts UI and internal state after stopping demo mode.
 *
 * This function stops the demo simulator and style rotator, restores the original
 * stylesheet, re-enables network UI controls, and resets the tab state.
 */
    void stopDemoModeUiReset();

private slots:

    /** @name Help & About Actions
     *  Slots handling access to About dialogs and documentation.
     */
    ///@{
    /**
     * @brief Shows the application's custom About dialog.
     */
    void on_actionAbout_triggered();

    /**
     * @brief Displays the standard Qt About dialog.
     */
    void on_actionAbout_Qt_triggered();

    /**
     * @brief Opens the Qt license document (LGPLv3.pdf) from resources.
     */
    void on_actionQt_License_triggered();

    /**
     * @brief Opens the user manual (UserManual.pdf) from resources.
     */
    void on_actionUser_Manual_triggered();
    ///@}

    /** @name Chat Interaction
     *  Slots related to composing and sending messages.
     */
    ///@{
    /**
     * @brief Triggered when the Send button is clicked.
     * Sends the current message entered in the chat input field.
     */
    void on_pushButtonSend_clicked();

    /**
     * @brief Triggered when Return/Enter is pressed in the chat input field.
     * Sends the message if the send button is enabled.
     */
    void on_lineEditChatText_returnPressed();

    /**
     * @brief Triggered when the "Test Message" button is clicked.
     * Inserts a test message into the chat input for simulation or testing purposes.
     */
    void on_pushButtonTestMsg_clicked();
    ///@}

    /** @name UDP Connection Control
     *  Establishes and tears down UDP communication channels.
     */
    ///@{
    /**
     * @brief Attempts to connect the application to the configured UDP socket.
     */
    void on_pushButtonConnect_clicked();

    /**
     * @brief Disconnects from the current UDP socket session and resets the UI.
     */
    void on_pushButtonDisconnect_clicked();
    ///@}

    /** @name Network Configuration
     *  Updates for local and remote network settings.
     */
    ///@{
    void on_checkBoxMulticast_clicked(bool checked);
    void on_checkBoxLoopback_clicked(bool checked);
    void on_spinBoxTTL_valueChanged(int arg1);
    void on_comboBoxLocalUDPNetwork_currentTextChanged(const QString &arg1);
    void on_lineEditLocalUDPPort_textChanged(const QString &arg1);
    void on_lineEditRemoteUDPNetwork_textChanged(const QString &arg1);
    void on_lineEditRemoteUDPPort_textChanged(const QString &arg1);
    ///@}

    /** @name Appearance Settings
     *  Style customization, including background and theme selection.
     */
    ///@{
    void on_comboBoxSelectStyleSheet_currentTextChanged(const QString &arg1);
    void on_checkBoxLoadStyleSheet_clicked(bool checked);
    void on_checkBoxDisplayBackgroundImage_clicked(bool checked);
    ///@}

    /** @name User Preferences
     *  Updates to user display name and personalization options.
     */
    ///@{
    void on_lineEditUserName_textChanged(const QString &arg1);
    ///@}

    /** @name Database Management
     *  Handles message history scrolling and database clearing.
     */
    ///@{
    /**
     * @brief Triggered when the chat scrollbar value changes.
     * Detects the scroll direction and loads messages accordingly.
     */
    void onChatScrollBarChanged(int value);

    /**
     * @brief Triggered when the "Delete Database" button is clicked.
     * Prompts the user and clears all stored messages if confirmed.
     */
    void on_pushButtonDeleteDatabase_clicked();
    ///@}
    /**
 * @brief Starts or stops the demo mode.
 *
 * This slot is triggered when the "Start Demo Mode" or "Stop Demo Mode" button is clicked.
 * If demo mode is not currently running:
 * - Ensures the application is disconnected from the network.
 * - Stores the current stylesheet as a fallback.
 * - Attempts to start the demo simulator and stylesheet rotator.
 * - Updates the UI to reflect demo state.
 *
 * If demo mode is already running:
 * - Stops the demo simulator and stylesheet rotator.
 * - Restores the previously active stylesheet.
 * - Reloads the most recent messages from the database.
 * - Re-enables network and chat input UI controls.
 *
 * Demo mode simulates an automated chat sequence and theme rotation for visual demonstration.
 */
    void on_pushButtonStartStopDemo_clicked();
};

#endif // MAINWINDOW_H
