#ifndef INSTANCEIDMANAGER_H
#define INSTANCEIDMANAGER_H

#include <QString>
#include <QMap>

class InstanceIdManager {
public:
    explicit InstanceIdManager(const QString &appDirPath);

    int acquire();
    void release(int instanceId);

private:
    QString idsFilePath() const;
    QString lockFilePath() const;

    QMap<int, int> loadValidIdPidMapExcluding(int excludeId) const;
    void saveIdPidMap(const QMap<int, int> &idPidMap) const;
    bool isProcessRunning(int pid) const;

    QString basePath;
};

#endif // INSTANCEIDMANAGER_H
