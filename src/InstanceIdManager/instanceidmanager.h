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




#ifndef INSTANCEIDMANAGER_H
#define INSTANCEIDMANAGER_H

#include <QString>
#include <QMap>

/**
 * @brief Manages unique instance identifiers for running application instances.
 *
 * This class ensures that each running instance of an application is assigned a unique ID.
 * It stores instance IDs and their associated process IDs in a persistent file,
 * using a lock file to synchronize concurrent access.
 */
class InstanceIdManager {
public:
    /**
     * @brief Constructs an InstanceIdManager.
     * @param appDirPath The directory path where instance IDs and lock files are stored.
     */
    explicit InstanceIdManager(const QString &appDirPath);

    /**
     * @brief Acquires a new unique instance ID for this process.
     *
     * Locks the IDs file, loads existing valid IDs (excluding none),
     * determines the smallest unused positive integer, associates it with the
     * current process ID, writes the updated map back to disk, and releases the lock.
     *
     * @return The newly acquired instance ID (>= 1), or -1 on error (e.g., lock failure).
     */
    int acquire();

    /**
     * @brief Releases a previously acquired instance ID.
     *
     * Locks the IDs file, loads existing valid IDs while excluding the given ID,
     * writes the remaining IDs back to disk, and releases the lock.
     *
     * @param instanceId The instance ID to release. If < 1, this call is a no-op.
     */
    void release(int instanceId);

private:
    /**
     * @brief Returns the full path to the file storing instance IDs.
     * @return Absolute path to the instance IDs file.
     */
    QString idsFilePath() const;

    /**
     * @brief Returns the full path to the lock file for synchronizing ID access.
     * @return Absolute path to the lock file.
     */
    QString lockFilePath() const;

    /**
     * @brief Loads the map of valid instance IDs to process IDs from disk.
     *
     * Reads the IDs file line by line, parsing entries of the form "id:pid".
     * Excludes the entry whose ID equals @p excludeId and only includes entries
     * for which the associated process is still running.
     *
     * @param excludeId An instance ID to omit from the returned map.
     * @return A map of valid instance IDs to their corresponding process IDs.
     */
    QMap<int, int> loadValidIdPidMapExcluding(int excludeId) const;

    /**
     * @brief Saves the given map of instance IDs to process IDs to disk.
     *
     * Overwrites the IDs file, writing each entry as "id:pid\n".
     *
     * @param idPidMap The map of instance IDs to process IDs to save.
     */
    void saveIdPidMap(const QMap<int, int> &idPidMap) const;

    /**
     * @brief Checks whether a process with the given PID is still running.
     *
     * On Windows, opens a handle to the process and waits with zero timeout.
     * On Unix-like systems, sends signal 0 to check for process existence.
     *
     * @param pid The process ID to check.
     * @return true if the process is running; false otherwise.
     */
    bool isProcessRunning(int pid) const;

    QString basePath; /**< Base directory path for storing instance ID files. */
};

#endif // INSTANCEIDMANAGER_H
