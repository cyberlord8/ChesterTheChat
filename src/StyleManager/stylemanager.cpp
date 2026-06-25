/*
 * Chester The Chat
 * Copyright (C) 2024 - 2026 Timothy Millea
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

#include "stylemanager.h"


#include <QApplication>
#include <QFile>
#include <QTextStream>

StyleManager::StyleManager() {}

void StyleManager::setStyleSheetMap(const QMap<QString, QString> &map) {
    styleSheetMap = map;
}//

bool StyleManager::loadStyleSheet(const QString &name)
{
    if (!styleSheetMap.contains(name)){
        qApp->setStyleSheet("");
        return true;
    }

    currentStyle = name;
    loadedStyle = readStyleSheetFile(styleSheetMap.value(name));
    if (loadedStyle.isEmpty()) return false;
    qApp->setStyleSheet(loadedStyle);
    return true;
}//

QString StyleManager::readStyleSheetFile(const QString &path) const {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";
    QTextStream in(&file);
    return in.readAll();
}//

QString StyleManager::currentStyleName() const {
    return currentStyle;
}
