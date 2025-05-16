#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

struct Settings {
    QString stylesheetName;
    bool b_loadStyleSheet = false;
    bool b_displayBackgroundImage = false;

    QString localUDPAddress;
    QString remoteUDPAddress;
    quint16 localUDPPort = 12345;
    quint16 remoteUDPPort = 12345;
    int udpTTL = 1;
    bool b_multicast = false;
    bool b_loopback = false;
    bool b_isDarkThemed = false;

    QString userName;
};

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(int instanceID, const QString &appPath, QObject *parent = nullptr);
    void load(Settings &s);
    void save(const Settings &s);

    void saveGeometry(const QByteArray &geometry);
    QByteArray loadGeometry() const;

    template <typename T>
    static void update(T &settingRef, const T &newValue) {
        if (settingRef != newValue) {
            settingRef = newValue;
        }
    }

private:
    int m_instanceID;
    QString m_appPath;
};

#endif // SETTINGSMANAGER_H
