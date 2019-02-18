#include "mainwindow.h"
#include <QApplication>
#include <QThread>

#include "getfpgadata.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.startAllTh();






    return a.exec();
}
