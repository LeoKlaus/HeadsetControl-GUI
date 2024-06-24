QT += core gui
QT += core network

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Device.cpp \
    ghTools.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Device.h \
    ghTools.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    HeadsetControl-GUI_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

RC_FILE = myapp.rc

DISTFILES += \
    .gitignore \
    headphones-exe.ico \
    myapp.rc
