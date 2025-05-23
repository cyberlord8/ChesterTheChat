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

#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>

int main(int argc, char *argv[])
{
#ifdef WIN32
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
#endif
    QApplication a(argc, argv);
    MainWindow w;

    QPixmap pixmap(":/images/BackgroundImage10.png");
    pixmap = pixmap.scaled(512,512,Qt::KeepAspectRatio);

    QSplashScreen splash(pixmap);
    splash.show();
    splash.raise();

    splash.finish(&w);
    QThread::msleep(1500);
    w.show();
    return a.exec();
}
