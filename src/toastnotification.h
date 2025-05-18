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
    static constexpr int kDisplayDurationMs = 5000;
    static constexpr int kPadding = 20;
    void setupLabel(const QString &text);
    void setupLayout();
    void positionBottomRight();
};

#endif // TOASTNOTIFICATION_H
