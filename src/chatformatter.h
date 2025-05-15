#ifndef CHATFORMATTER_H
#define CHATFORMATTER_H

#include <QObject>
#include <QMap>
#include <QColor>
#include <QString>
#include <QDateTime>

class QTextEdit;

class ChatFormatter : public QObject
{
    Q_OBJECT
public:
    explicit ChatFormatter(QObject *parent = nullptr);

    // The only public API: append a chat line to a QTextEdit
    void appendMessage(QTextEdit *textEdit,
                       const QString &user,
                       const QString &message,
                       const QDateTime &timestamp,
                       bool isSent);

private:
    QColor generateColorForUser(const QString &user);
    bool isDarkTheme(const QTextEdit *textEdit) const;

    QMap<QString, QColor> userColorMap;
};

#endif // CHATFORMATTER_H
