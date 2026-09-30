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

#include "stylerotator.h"
#include "../Utils/debugmacros.h"

#include <QDir>
#include <QStringList>

StyleRotator::StyleRotator(QComboBox *comboBox, QMap<QString, QString> styleMap, QObject *parent)
    : QObject(parent)
    , comboBox(comboBox)
    , styleSheetMap(styleMap)
{
    LOG_DEBUG("");

    connect(&rotateTimer, &QTimer::timeout, this, &StyleRotator::applyNextStyle);
}//StyleRotator

void StyleRotator::start()
{
    LOG_DEBUG("");

    if (styleSheetMap.isEmpty())
        return;
    currentIndex = 0;
    applyNextStyle();
    rotateTimer.start(10000);
}//start

void StyleRotator::stop()
{
    LOG_DEBUG("");

    rotateTimer.stop();
}//stop

void StyleRotator::applyNextStyle()
{
    if (styleSheetMap.isEmpty() || !comboBox)
        return;

    const QStringList keys = styleSheetMap.keys();

    for (int checked = 0; checked < keys.size(); ++checked) {
        const int index = (currentIndex + checked) % keys.size();
        const QString &key = keys.at(index);
        const QString path =
            QDir::fromNativeSeparators(styleSheetMap.value(key));

        if (path.contains("/Chester/")) {
            comboBox->setCurrentText(key);
            currentIndex = (index + 1) % keys.size();
            return;
        }
    }

    // No eligible stylesheet; leave the current style unchanged.
} // applyNextStyle

// void StyleRotator::applyNextStyle()
// {
//     LOG_DEBUG("");

//     if (styleSheetMap.isEmpty())
//         return;

//     static uint index = 0;
//     while (!styleSheetMap.value(styleSheetMap.keys().at(index)).contains("/Chester/")) {

//         index++;

//         if(index >= styleSheetMap.size()){
//             index = 0;
//         }
//     }

//     comboBox->setCurrentText(styleSheetMap.keys().at(index++));

//     if(index >= styleSheetMap.size()){
//         index = 0;
//     }
// }//applyNextStyle
