#include "StyleRotator.h"
#include "src/debugmacros.h"

#include <QComboBox>

StyleRotator::StyleRotator(QComboBox *comboBox, QMap<QString, QString> styleMap, QObject *parent)
    : QObject(parent)
    , comboBox(comboBox)
    , styleSheetMap(styleMap)
{
    LOG_DEBUG(Q_FUNC_INFO);

    connect(&rotateTimer, &QTimer::timeout, this, &StyleRotator::applyNextStyle);
}//StyleRotator

void StyleRotator::start()
{
    LOG_DEBUG(Q_FUNC_INFO);

    if (styleSheetMap.isEmpty())
        return;
    currentIndex = 0;
    applyNextStyle();
    rotateTimer.start(10000);
}//start

void StyleRotator::stop()
{
    LOG_DEBUG(Q_FUNC_INFO);

    rotateTimer.stop();
}//stop

void StyleRotator::applyNextStyle()
{
    LOG_DEBUG(Q_FUNC_INFO);

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
