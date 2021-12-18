#include "mainwindow.h"

#include <QApplication>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(10, 10);
    w.show();

    return a.exec();
}
