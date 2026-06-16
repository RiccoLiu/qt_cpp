
#include <QApplication>

#include "mainwindow.h"
#include "database.h"
#include "logger2.h"

int main(int argc, char *argv[])
{
    Logger2::Init(Logger2::Level::Info, "mysql.log");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

