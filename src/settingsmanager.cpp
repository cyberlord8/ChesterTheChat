#include "SettingsManager.h"

SettingsManager::SettingsManager(int instanceID, const QString &appPath, QObject *parent)
    : QObject(parent), m_instanceID(instanceID), m_appPath(appPath)
{}

void SettingsManager::load(Settings &s)
{
    // QSettings settings;
    const QString settingsFile = m_appPath +
                                 QString("/instance_%1_settings.ini").arg(m_instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    s.stylesheetName = settings.value("StyleSheetName", "").toString();
    s.b_loadStyleSheet = settings.value("LoadStyleSheet", false).toBool();
    s.b_displayBackgroundImage = settings.value("DisplayBackgroundImage", false).toBool();
    s.localUDPAddress = settings.value("LocalUDPAddress", "0.0.0.0").toString();
    s.remoteUDPAddress = settings.value("RemoteUDPAddress", "239.0.0.1").toString();
    s.localUDPPort = settings.value("LocalUDPPort", 12345).toUInt();
    s.remoteUDPPort = settings.value("RemoteUDPPort", 12345).toUInt();
    s.udpTTL = settings.value("UdpTTL", 5).toInt();
    s.b_multicast = settings.value("Multicast", true).toBool();
    s.b_loopback = settings.value("Loopback", true).toBool();
    s.userName = settings.value("UserName", "Chester").toString();
}

void SettingsManager::save(const Settings &s)
{
    // QSettings settings;
    const QString settingsFile = m_appPath +
                                 QString("/instance_%1_settings.ini").arg(m_instanceID);

    QSettings settings(settingsFile, QSettings::IniFormat, this);

    settings.setValue("StyleSheetName", s.stylesheetName);
    settings.setValue("LoadStyleSheet", s.b_loadStyleSheet);
    settings.setValue("DisplayBackgroundImage", s.b_displayBackgroundImage);
    settings.setValue("LocalUDPAddress", s.localUDPAddress);
    settings.setValue("RemoteUDPAddress", s.remoteUDPAddress);
    settings.setValue("LocalUDPPort", s.localUDPPort);
    settings.setValue("RemoteUDPPort", s.remoteUDPPort);
    settings.setValue("UdpTTL", s.udpTTL);
    settings.setValue("Multicast", s.b_multicast);
    settings.setValue("Loopback", s.b_loopback);
    settings.setValue("UserName", s.userName);
}

void SettingsManager::saveGeometry(const QByteArray &geometry)
{
    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    settings.setValue("WindowGeometry", geometry);
}

QByteArray SettingsManager::loadGeometry() const
{
    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    return settings.value("WindowGeometry").toByteArray();
}

