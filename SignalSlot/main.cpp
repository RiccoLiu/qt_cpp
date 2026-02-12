#include "maindialog.h"

#include <QApplication>
#include <iostream>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::cout << "---- start ---" << std::endl;
    MainDialog w;
    w.show();
    return a.exec();
}
