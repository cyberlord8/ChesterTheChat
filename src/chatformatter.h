#ifndef CHATFORMATTER_H
#define CHATFORMATTER_H

#include <QObject>
#include <QMap>
#include <QColor>
#include <QString>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "settingsmanager.h"

class QTextEdit;

class ChatFormatter : public QObject
{
    Q_OBJECT
public:
    explicit ChatFormatter(QObject *parent = nullptr);

    void appendMessage(QTextEdit *textEdit, const QString &user, const QString &message, const QDateTime &timestamp, bool isDarkThemed, bool isSent);

private:
    QColor generateColorForUser(const QString &user);
    QMap<QString, QColor> userColorMap;
};

#endif // CHATFORMATTER_H
