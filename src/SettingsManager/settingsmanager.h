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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QSettings>

class QString;
/**
 * @struct Settings
 * @brief Represents user-configurable application settings.
 *
 * This struct contains UI preferences, network configuration,
 * and user identity settings used by the application at runtime.
 */
struct Settings {
    /** @brief Path to the selected stylesheet (.qss) file. */
    QString stylesheetName;

    /** @brief Whether to load the selected stylesheet at startup. */
    bool b_loadStyleSheet = false;

    /** @brief Whether to display a background image in the chat view. */
    bool b_displayBackgroundImage = false;

    /** @brief The local IP address for UDP communication. */
    QString localUDPAddress;

    /** @brief The remote multicast or unicast IP address. */
    QString remoteUDPAddress;

    /** @brief The local UDP port to bind for receiving. */
    quint16 localUDPPort = 12345;

    /** @brief The remote UDP port to send messages to. */
    quint16 remoteUDPPort = 12345;

    /** @brief Time-To-Live for UDP multicast packets. */
    int udpTTL = 1;

    // /** @brief Whether to enable multicast mode for outgoing UDP. */
    // bool b_multicast = false;

    /** @brief Whether to allow receiving own sent UDP messages (loopback). */
    bool b_loopback = false;

    /** @brief The display name of the user. */
    QString userName;
};


/**
 * @class SettingsManager
 * @brief Manages saving and loading of user settings and application geometry.
 *
 * SettingsManager handles persistence of runtime configuration such as
 * network parameters, UI preferences, and user identity. Each instance can manage
 * settings for a specific application instance using a unique settings file.
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new SettingsManager.
     * @param instanceID An ID representing the running instance (for multi-instance support).
     * @param appPath The base directory where settings files are stored.
     * @param parent The parent QObject.
     */
    explicit SettingsManager(int instanceID, const QString &appPath, QObject *parent = nullptr);

    /**
     * @brief Loads application settings into the provided Settings struct.
     * @param s Reference to the Settings object to populate.
     */
    void load(Settings &s);

    /**
     * @brief Saves the current Settings struct to persistent storage.
     * @param s The settings to persist.
     */
    void save(const Settings &s);

    /**
     * @brief Saves the main window geometry (size and position).
     * @param geometry A QByteArray representing the window geometry.
     */
    void saveGeometry(const QByteArray &geometry);

    /**
     * @brief Loads the previously saved window geometry.
     * @return A QByteArray containing the saved geometry data.
     */
    QByteArray loadGeometry() const;

    /**
     * @brief Updates a setting reference only if the new value is different.
     * @tparam T The type of the setting.
     * @param settingRef Reference to the setting field to update.
     * @param newValue New value to assign.
     */
    template <typename T>
    static void update(T &settingRef, const T &newValue) {
        if (settingRef != newValue) {
            settingRef = newValue;
        }
    }

private:
    /** @brief Instance ID used to isolate settings files. */
    int m_instanceID;

    /** @brief Application base path for settings file storage. */
    QString m_appPath;
};


#endif // SETTINGSMANAGER_H
