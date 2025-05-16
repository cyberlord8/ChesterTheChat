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
