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




#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>
#include <QMap>
#include <QTextEdit>

/**
 * @file stylemanager.h
 * @brief Declares StyleManager for loading and applying Qt stylesheets.
 */

/**
 * @class StyleManager
 * @brief Loads .qss files and applies them to the QApplication.
 *
 * StyleManager maintains a mapping from human-readable style names to
 * stylesheet file paths. It can load a selected stylesheet and apply it
 * globally to the application, as well as report the currently active style.
 */
class StyleManager {
public:
    /**
     * @brief Constructs an empty StyleManager.
     *
     * Initializes with no stylesheet mapping and no currently loaded style.
     */
    StyleManager();

    /**
     * @brief Sets the map of available styles.
     * @param map A QMap where keys are display names and values are file paths to .qss files.
     */
    void setStyleSheetMap(const QMap<QString, QString> &map);

    /**
     * @brief Loads and applies the stylesheet identified by @p name.
     *
     * If @p name is not found in the map, clears any existing stylesheet.
     * Otherwise, reads the file at the mapped path and applies its contents
     * to the QApplication.
     *
     * @param name The human-readable name of the stylesheet to load.
     * @return true if the stylesheet was successfully loaded or cleared;
     *         false if the file could not be read or was empty.
     */
    bool loadStyleSheet(const QString &name);

    /**
     * @brief Returns the name of the currently applied stylesheet.
     * @return The display name of the loaded stylesheet, or an empty string if none.
     */
    QString currentStyleName() const;

private:
    /**
     * @brief Reads the contents of a stylesheet file.
     *
     * Opens the file at @p path in read-only text mode and returns its contents
     * as a QString. Returns an empty string on failure.
     *
     * @param path Filesystem path to the .qss file.
     * @return The full text of the stylesheet, or empty if unreadable.
     */
    QString readStyleSheetFile(const QString &path) const;

    QString loadedStyle;               /**< Raw contents of the last loaded stylesheet. */
    QMap<QString, QString> styleSheetMap; /**< Mapping of style names to .qss file paths. */
    QString currentStyle;              /**< Display name of the currently loaded style. */
};

#endif // STYLEMANAGER_H
