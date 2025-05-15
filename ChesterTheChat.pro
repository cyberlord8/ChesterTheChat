QT       += core gui network

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
    src/mainwindow.ui

HEADERS += \
    src/chatformatter.h \
    src/globals.h \
    src/mainwindow.h \
    src/udpchatsocketmanager.h \
    src/version.h

SOURCES += \
    src/chatformatter.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/udpchatsocketmanager.cpp
