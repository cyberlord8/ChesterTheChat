#include "instanceidmanager.h"

#include <QFile>
#include <QTextStream>
#include <QLockFile>
#include <QDebug>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
bool InstanceIdManager::isProcessRunning(int pid) const {
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!hProcess)
        return false;
    DWORD ret = WaitForSingleObject(hProcess, 0);
    CloseHandle(hProcess);
    return ret == WAIT_TIMEOUT;
}//isProcessRunning
#else
#include <signal.h>
bool InstanceIdManager::isProcessRunning(int pid) const {
    return (kill(pid, 0) == 0);
}
#endif

InstanceIdManager::InstanceIdManager(const QString &appDirPath)
    : basePath(appDirPath)
{}//InstanceIdManager

QString InstanceIdManager::idsFilePath() const {
    return basePath + "/instance_ids.txt";
}//idsFilePath

QString InstanceIdManager::lockFilePath() const {
    return basePath + "/instance_ids.lock";
}//lockFilePath

QMap<int, int> InstanceIdManager::loadValidIdPidMapExcluding(int excludeId) const {
    QMap<int, int> idPidMap;
    QFile file(idsFilePath());

    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return idPidMap;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        const QStringList parts = line.split(':');
        if (parts.size() != 2)
            continue;

        int id = parts[0].toInt();
        int pid = parts[1].toInt();

        if (id != excludeId && isProcessRunning(pid))
            idPidMap.insert(id, pid);
    }

    file.close();
    return idPidMap;
}//loadValidIdPidMapExcluding

void InstanceIdManager::saveIdPidMap(const QMap<int, int> &idPidMap) const {
    QFile file(idsFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream out(&file);
    for (auto it = idPidMap.begin(); it != idPidMap.end(); ++it) {
        out << it.key() << ":" << it.value() << "\n";
    }

    file.close();
}//saveIdPidMap

int InstanceIdManager::acquire() {
    QLockFile lock(lockFilePath());
    if (!lock.tryLock(5000)) {
        qWarning() << "[InstanceIdManager] Failed to lock for acquiring ID.";
        return -1;
    }

    QMap<int, int> validIdPidMap = loadValidIdPidMapExcluding(-1);
    int newId = 1;
    while (validIdPidMap.contains(newId))
        ++newId;

    int currentPid = QCoreApplication::applicationPid();
    validIdPidMap.insert(newId, currentPid);
    saveIdPidMap(validIdPidMap);

    lock.unlock();
    return newId;
}//acquire

void InstanceIdManager::release(int instanceId) {
    if (instanceId < 1)
        return;

    QLockFile lock(lockFilePath());
    if (!lock.tryLock(5000)) {
        qWarning() << "[InstanceIdManager] Failed to lock for releasing ID.";
        return;
    }

    QMap<int, int> remaining = loadValidIdPidMapExcluding(instanceId);
    saveIdPidMap(remaining);

    lock.unlock();
}//release
