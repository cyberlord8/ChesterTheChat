#ifndef GLOBALS_H
#define GLOBALS_H

#include "version.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>

#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopServices>
#include <QSet>

#include <QSettings>
#include <QDateTime>
#include <QString>
#include <QStringList>

#include <QNetworkInterface>
#include <QHostAddress>
#include <QUdpSocket>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QLockFile>


#define APP_NAME "Chester the Chat"
#define RELEASEDATE __DATE__
#define BUILDDATE RELEASEDATE " - " __TIME__
#define SYSTEMINFO QSysInfo::prettyProductName()
#define COPYRIGHT "Copyright © "
#define COMPANY " GRSS"
#define RIGHTS "All rights reserved"
#define WARRANTY "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
#define qt_COPYRIGHT "Copyright © The Qt Company Ltd. and other contributors"
#define APP_LICENSE "Green Radio Software Solutions (GRSS) grants anyone license to use this software for personal use."
#define qt_LICENSE "LGPLv3"


struct Message {
    QString user;
    QString text;
    QDateTime timestamp;
    bool isSentByMe = false;
};



#endif // GLOBALS_H
