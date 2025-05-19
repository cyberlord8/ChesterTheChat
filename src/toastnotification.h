#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

/**
 * @class ToastNotification
 * @brief Displays a temporary, non-intrusive notification on the screen.
 *
 * ToastNotification is a lightweight popup widget styled similarly to mobile toast messages.
 * It automatically disappears after a short duration and is typically used to provide
 * transient feedback to the user (e.g., message received, error alert).
 */
class ToastNotification : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs and displays a toast notification.
     * @param text The text message to display.
     * @param parent The parent widget (optional).
     */
    explicit ToastNotification(const QString &text, QWidget *parent = nullptr);

private:
    /**
     * @brief Internal timer that triggers the auto-close behavior.
     */
    QTimer timer;

    /**
     * @brief Label used to display the toast text.
     */
    QLabel *label;

    /**
     * @brief Duration in milliseconds the toast is displayed.
     */
    static constexpr int kDisplayDurationMs = 5000;

    /**
     * @brief Padding from the screen edges when positioning the toast.
     */
    static constexpr int kPadding = 20;

    /**
     * @brief Configures and styles the toast label.
     * @param text The message to display.
     */
    void setupLabel(const QString &text);

    /**
     * @brief Sets up the widget layout.
     */
    void setupLayout();

    /**
     * @brief Positions the toast in the bottom-right corner of the primary screen.
     */
    void positionBottomRight();
};


#endif // TOASTNOTIFICATION_H
