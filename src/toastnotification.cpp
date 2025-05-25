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

#include "ToastNotification.h"
#include "src/debugmacros.h"

#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>

void ToastNotification::setupLabel(const QString &text)
{
    LOG_DEBUG(Q_FUNC_INFO);

    label = new QLabel(text, this);
    label->setStyleSheet(R"(
        background-color: rgba(0, 0, 0, 180);
        color: white;
        padding: 8px;
        border-radius: 6px;
    )");
}//setupLabel

void ToastNotification::setupLayout()
{
    LOG_DEBUG(Q_FUNC_INFO);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}//setupLayout

void ToastNotification::positionBottomRight()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.right() - width() - kPadding,
             screenGeometry.bottom() - height() - kPadding);
    }
}//positionBottomRight

ToastNotification::ToastNotification(const QString &text, QWidget *parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
{
    LOG_DEBUG(Q_FUNC_INFO);

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);

    setupLabel(text);
    setupLayout();
    adjustSize();
    positionBottomRight();

    show();

    connect(&timer, &QTimer::timeout, this, &QWidget::close);
    timer.start(kDisplayDurationMs);
}//ToastNotification

