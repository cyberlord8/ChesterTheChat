#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

class ToastNotification : public QWidget {
    Q_OBJECT
public:
    explicit ToastNotification(const QString &text, QWidget *parent = nullptr);

private:
    QTimer timer;
    QLabel *label;
};

#endif // TOASTNOTIFICATION_H
