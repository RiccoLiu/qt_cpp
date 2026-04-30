
#include <QApplication>

#include "mainwindow.h"
#include "database.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    logger_enable_file_logging("mysql.log", 1024, 1);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
