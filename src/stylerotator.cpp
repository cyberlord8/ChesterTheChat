#include "StyleRotator.h"

StyleRotator::StyleRotator(QComboBox *comboBox, QMap<QString, QString> styleMap, QObject *parent)
    : QObject(parent)
    , comboBox(comboBox)
    , styleSheetMap(styleMap)
{
    connect(&rotateTimer, &QTimer::timeout, this, &StyleRotator::applyNextStyle);
}//StyleRotator

void StyleRotator::start()
{
    if (styleSheetMap.isEmpty())
        return;
    currentIndex = 0;
    applyNextStyle();
    rotateTimer.start(10000);
}//start

void StyleRotator::stop()
{
    rotateTimer.stop();
}//stop

void StyleRotator::applyNextStyle()
{
    if (styleSheetMap.isEmpty())
        return;

    static uint index = 0;
    while (!styleSheetMap.value(styleSheetMap.keys().at(index)).contains("/Chester/")) {

        index++;

        if(index >= styleSheetMap.size()){
            index = 0;
        }
    }

    qDebug() << "Loading demo stylesheet:" << styleSheetMap.value(styleSheetMap.keys().at(index));
    comboBox->setCurrentText(styleSheetMap.keys().at(index++));

    if(index >= styleSheetMap.size()){
        index = 0;
    }
}//applyNextStyle
