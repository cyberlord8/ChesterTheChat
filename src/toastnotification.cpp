#include "ToastNotification.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>

void ToastNotification::setupLabel(const QString &text)
{
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
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}//setupLayout

void ToastNotification::positionBottomRight()
{
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.right() - width() - kPadding,
             screenGeometry.bottom() - height() - kPadding);
    }
}//positionBottomRight

ToastNotification::ToastNotification(const QString &text, QWidget *parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
{
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

