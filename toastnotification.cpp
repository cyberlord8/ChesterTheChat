#include "ToastNotification.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>

ToastNotification::ToastNotification(const QString &text, QWidget *parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);

    label = new QLabel(text, this);
    label->setStyleSheet("background-color: rgba(0,0,0,180); color: white; padding: 8px; border-radius: 6px;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    setLayout(layout);
    adjustSize();

    // Position bottom-right
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.right() - width() - 20,
             screenGeometry.bottom() - height() - 20);
    }


    show();

    connect(&timer, &QTimer::timeout, this, &QWidget::close);
    timer.start(3000);
}//
