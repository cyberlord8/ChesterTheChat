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

StyleRotator::StyleRotator(QComboBox *comboBox, QMap<QString, QString> styleMap, QObject *parent)
    : QObject(parent)
    , comboBox(comboBox)
    , styleSheetMap(styleMap)
{
    LOG_DEBUG(Q_FUNC_INFO);

    connect(&rotateTimer, &QTimer::timeout, this, &StyleRotator::applyNextStyle);
}//StyleRotator

void StyleRotator::start()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (styleSheetMap.isEmpty())
        return;
    currentIndex = 0;
    applyNextStyle();
    rotateTimer.start(10000);
}//start

void StyleRotator::stop()
{
    LOG_DEBUG(Q_FUNC_INFO);

    rotateTimer.stop();
}//stop

void StyleRotator::applyNextStyle()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (styleSheetMap.isEmpty())
        return;

    static uint index = 0;
    while (!styleSheetMap.value(styleSheetMap.keys().at(index)).contains("/Chester/")) {

        index++;

        if(index >= styleSheetMap.size()){
            index = 0;
        }
    }

    comboBox->setCurrentText(styleSheetMap.keys().at(index++));

    if(index >= styleSheetMap.size()){
        index = 0;
    }
}//applyNextStyle
