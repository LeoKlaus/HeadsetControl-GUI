QT += core gui network
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += static c++11
QMAKE_LFLAGS += -static

SOURCES += \
    Device.cpp \
    dialoginfo.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    settingswindow.cpp \
    utils.cpp

HEADERS += \
    Device.h \
    dialoginfo.h \
    mainwindow.h \
    settings.h \
    settingswindow.h \
    utils.h

FORMS += \
    dialoginfo.ui \
    mainwindow.ui \
    settingswindow.ui

TRANSLATIONS += \
    HeadsetControl_GUI_en_US.ts

RESOURCES += \
    icons.qrc

RC_FILE = myapp.rc

DISTFILES += \
    .gitignore \
    headphones-exe.ico \
    myapp.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
