//#include <QtGui/QApplication>   //Use this for Qt 4.x.x
#include <QtWidgets>              //Use this for Qt 5.x.x
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
