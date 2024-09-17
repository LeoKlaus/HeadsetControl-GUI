#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

const QString APP_NAME = "HeadsetControl-GUI";
const QString GUI_VERSION = "0.16.4";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(GUI_VERSION);
    QLocale locale = QLocale::system();
    QString languageCode = locale.name();
    QTranslator translator;
    if (translator.load(":/translations/tr/HeadsetControl_GUI_" + languageCode + ".qm")) {
        app.installTranslator(&translator);
    }
    MainWindow window;

    return app.exec();
}
