QT += core gui network
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += static c++17

INCLUDEPATH += \
    src/DataTypes \
    src/UI \
    src/Utils

SOURCES += \
    src/main.cpp \
    src/DataTypes/device.cpp \
    src/DataTypes/settings.cpp \
    src/UI/dialoginfo.cpp \
    src/UI/loaddevicewindow.cpp \
    src/UI/mainwindow.cpp \
    src/UI/settingswindow.cpp \
    src/Utils/utils.cpp

HEADERS += \
    src/DataTypes/device.h \
    src/DataTypes/settings.h \
    src/UI/dialoginfo.h \
    src/UI/loaddevicewindow.h \
    src/UI/mainwindow.h \
    src/UI/settingswindow.h \
    src/Utils/utils.h

FORMS += \
    src/UI/dialoginfo.ui \
    src/UI/loaddevicewindow.ui \
    src/UI/mainwindow.ui \
    src/UI/settingswindow.ui

TRANSLATIONS += \
    src/Resources/translations/HeadsetControl_GUI_en_US.ts

RESOURCES += \
    src/Resources/icons.qrc #\
    #src/Resources/translations.qrc

RC_FILE = src/Resources/appicon.rc

DISTFILES += \
    .gitignore \
    .github/VERSION.txt

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
