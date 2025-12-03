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

#include "settingsmanager.h"
#include "../Utils/debugmacros.h"

SettingsManager::SettingsManager(int instanceID, const QString &appPath, QObject *parent)
    : QObject(parent), m_instanceID(instanceID), m_appPath(appPath)
{
    LOG_DEBUG(Q_FUNC_INFO);

}

void SettingsManager::load(Settings &s)
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString settingsFile = QString("%1/instance_%2_settings.ini")
    .arg(m_appPath)
        .arg(m_instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    // Style settings
    s.stylesheetName = settings.value("StyleSheetName", "").toString();
    s.b_loadStyleSheet = settings.value("LoadStyleSheet", false).toBool();
    s.b_displayBackgroundImage = settings.value("DisplayBackgroundImage", false).toBool();

    // Network settings
    s.localUDPAddress = settings.value("LocalUDPAddress", "0.0.0.0").toString();
    s.remoteUDPAddress = settings.value("RemoteUDPAddress", "224.0.0.2").toString();
    s.localUDPPort = settings.value("LocalUDPPort", 9998).toUInt();
    s.remoteUDPPort = settings.value("RemoteUDPPort", 9998).toUInt();
    s.udpTTL = settings.value("UdpTTL", 5).toInt();
    // s.b_multicast = settings.value("Multicast", true).toBool();
    s.b_loopback = settings.value("Loopback", true).toBool();

    // Identity
    s.userName = settings.value("UserName", "Chester").toString();
}//load

void SettingsManager::save(const Settings &s)
{
    LOG_DEBUG(Q_FUNC_INFO);

    const QString settingsFile = QString("%1/instance_%2_settings.ini")
    .arg(m_appPath)
        .arg(m_instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    // Style settings
    settings.setValue("StyleSheetName", s.stylesheetName);
    settings.setValue("LoadStyleSheet", s.b_loadStyleSheet);
    settings.setValue("DisplayBackgroundImage", s.b_displayBackgroundImage);

    // Network settings
    settings.setValue("LocalUDPAddress", s.localUDPAddress);
    settings.setValue("RemoteUDPAddress", s.remoteUDPAddress);
    settings.setValue("LocalUDPPort", s.localUDPPort);
    settings.setValue("RemoteUDPPort", s.remoteUDPPort);
    settings.setValue("UdpTTL", s.udpTTL);
    // settings.setValue("Multicast", s.b_multicast);
    settings.setValue("Loopback", s.b_loopback);

    // Identity
    settings.setValue("UserName", s.userName);
}//save

void SettingsManager::saveGeometry(const QByteArray &geometry)
{
    LOG_DEBUG(Q_FUNC_INFO);

    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    settings.setValue("WindowGeometry", geometry);
}//saveGeometry

QByteArray SettingsManager::loadGeometry() const
{
    LOG_DEBUG(Q_FUNC_INFO);

    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    return settings.value("WindowGeometry").toByteArray();
}//loadGeometry

