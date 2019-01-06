#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication bioPlot(argc, argv);
    MainWindow mainWindow;

    mainWindow.show();
    return bioPlot.exec();
}


