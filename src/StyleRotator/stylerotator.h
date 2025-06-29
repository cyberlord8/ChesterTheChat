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

#ifndef STYLEROTATOR_H
#define STYLEROTATOR_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QComboBox>

class QComboBox;
class QString;

/**
 * @class StyleRotator
 * @brief Automatically rotates through available Chester style sheets for demo purposes.
 *
 * StyleRotator cycles through registered `.qss` files using a provided combo box,
 * simulating dynamic theme changes. It is typically used in demo mode to showcase
 * visual diversity of the application's styles (e.g., dark/light themes, Chester skins).
 */
class StyleRotator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a StyleRotator.
     * @param comboBox Pointer to the QComboBox widget controlling stylesheet selection.
     * @param styleMap A map of display names to stylesheet file paths.
     * @param parent Optional parent QObject for ownership.
     */
    explicit StyleRotator(QComboBox *comboBox, QMap<QString, QString> styleMap, QObject *parent = nullptr);

    /**
     * @brief Starts the automatic rotation of styles.
     *
     * Initiates periodic switching of the selected stylesheet in the combo box.
     * Styles are changed in sequence every few seconds to demonstrate UI variety.
     */
    void start();

    /**
     * @brief Stops the automatic rotation.
     *
     * Halts the timer responsible for advancing to the next style.
     */
    void stop();

private:
    /**
     * @brief Combo box linked to stylesheet selection.
     *
     * This widget reflects and controls the currently applied style in the UI.
     * Changing its selected item will trigger stylesheet loading via connected slots.
     */
    QComboBox *comboBox;

    /**
     * @brief List of style keys (display names) extracted from the style map.
     *
     * Used to cycle through available styles in a fixed order.
     */
    // QStringList styleKeys;
    QMap<QString, QString> styleSheetMap;

    /**
     * @brief Timer that drives periodic style rotation.
     */
    QTimer rotateTimer;

    /**
     * @brief Index of the currently selected style in the rotation list.
     */
    int currentIndex = 0;

    /**
     * @brief Applies the next stylesheet in the rotation sequence.
     *
     * Updates the combo box selection, triggering a UI update with the new style.
     */
    void applyNextStyle();
};


#endif // STYLEROTATOR_H
