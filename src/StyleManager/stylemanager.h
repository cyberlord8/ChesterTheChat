#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>
#include <QMap>
#include <QTextEdit>

class StyleManager {
public:
    StyleManager();

    void setStyleSheetMap(const QMap<QString, QString> &map);
    bool loadStyleSheet(const QString &name);
    QString currentStyleName() const;

    // void applyStyleToChat(QTextEdit *chatWidget);

private:
    QString readStyleSheetFile(const QString &path) const;
    QString loadedStyle;
    QMap<QString, QString> styleSheetMap;
    QString currentStyle;
};

#endif // STYLEMANAGER_H
