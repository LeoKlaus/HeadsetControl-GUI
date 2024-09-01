#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

const QString GUI_VERSION = "0.13.0";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationVersion(GUI_VERSION);
    QLocale language = QLocale::system();
    QString nation = language.amText();
    QTranslator translator;
    if (translator.load(":/translations/tr/HeadsetControl_GUI_it_IT.qm")) {
        app.installTranslator(&translator);
    }
    MainWindow window;


    return app.exec();
}
