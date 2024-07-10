QT += core gui network
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += static c++11
QMAKE_LFLAGS += -static

SOURCES += \
    Device.cpp \
    dialoginfo.cpp \
    ghTools.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Device.h \
    dialoginfo.h \
    ghTools.h \
    mainwindow.h

FORMS += \
    dialoginfo.ui \
    mainwindow.ui

TRANSLATIONS += \
    HeadsetControl-GUI_en_US.ts

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
