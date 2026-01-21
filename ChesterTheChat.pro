QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

    RC_ICONS = ./images/chester2.ico

FORMS += \
    src/MainWindow/mainwindow.ui

HEADERS += \
    ../Utils/debugmacros.h \
    src/ChatPager/chatpager.h \
    src/DemoChatSimulator/demochatsimulator.h \
    src/InstanceIdManager/instanceidmanager.h \
    src/MainWindow/mainwindow.h \
    src/StyleManager/stylemanager.h \
    src/features.h \
    src/MessageStore/messagestore.h \
    src/ChatFormatter/chatformatter.h \
    src/globals.h \
    src/SettingsManager/settingsmanager.h \
    src/StyleRotator/stylerotator.h \
    src/UDPChatSocketManager/udpchatsocketmanager.h \
    src/version.h \
    src/ToastNotification/toastnotification.h \
    todo.h

SOURCES += \
    src/ChatPager/chatpager.cpp \
    src/DemoChatSimulator/demochatsimulator.cpp \
    src/InstanceIdManager/instanceidmanager.cpp \
    src/MessageStore/messagestore.cpp \
    src/ChatFormatter/chatformatter.cpp \
    src/StyleManager/stylemanager.cpp \
    src/main.cpp \
    src/MainWindow/mainwindow.cpp \
    src/SettingsManager/settingsmanager.cpp \
    src/StyleRotator/stylerotator.cpp \
    src/UDPChatSocketManager/udpchatsocketmanager.cpp \
    src/ToastNotification/toastnotification.cpp

DISTFILES += \
    oldCode
    INCLUDEPATH += $$PWD/

