#include "SettingsManager.h"

SettingsManager::SettingsManager(int instanceID, const QString &appPath, QObject *parent)
    : QObject(parent), m_instanceID(instanceID), m_appPath(appPath)
{}

void SettingsManager::load(Settings &s)
{
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
    s.localUDPPort = settings.value("LocalUDPPort", 9999).toUInt();
    s.remoteUDPPort = settings.value("RemoteUDPPort", 9999).toUInt();
    s.udpTTL = settings.value("UdpTTL", 5).toInt();
    s.b_multicast = settings.value("Multicast", true).toBool();
    s.b_loopback = settings.value("Loopback", true).toBool();

    // Identity
    s.userName = settings.value("UserName", "Chester").toString();
}//load

void SettingsManager::save(const Settings &s)
{
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
    settings.setValue("Multicast", s.b_multicast);
    settings.setValue("Loopback", s.b_loopback);

    // Identity
    settings.setValue("UserName", s.userName);
}//save

void SettingsManager::saveGeometry(const QByteArray &geometry)
{
    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    settings.setValue("WindowGeometry", geometry);
}//saveGeometry

QByteArray SettingsManager::loadGeometry() const
{
    QSettings settings(m_appPath + QString("/instance_%1_settings.ini").arg(m_instanceID), QSettings::IniFormat);
    return settings.value("WindowGeometry").toByteArray();
}//loadGeometry

