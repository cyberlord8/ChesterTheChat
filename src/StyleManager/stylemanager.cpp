#include "stylemanager.h"


#include <QApplication>
#include <QFile>
#include <QTextStream>

StyleManager::StyleManager() {}

void StyleManager::setStyleSheetMap(const QMap<QString, QString> &map) {
    styleSheetMap = map;
}//

bool StyleManager::loadStyleSheet(const QString &name)
{
    if (!styleSheetMap.contains(name)){
        qApp->setStyleSheet("");
        return true;
    }

    currentStyle = name;
    loadedStyle = readStyleSheetFile(styleSheetMap.value(name));
    if (loadedStyle.isEmpty()) return false;
    qApp->setStyleSheet(loadedStyle);
    return true;
}//

QString StyleManager::readStyleSheetFile(const QString &path) const {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";
    QTextStream in(&file);
    return in.readAll();
}//

QString StyleManager::currentStyleName() const {
    return currentStyle;
}
