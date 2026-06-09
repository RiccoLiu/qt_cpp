
#include <QApplication>

#include "mainwindow.h"
#include "database2.h"
#include "logger2.h"

int main(int argc, char *argv[])
{
    Logger2::Init(Logger2::Level::Info, "mysql.log");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // 我想做一个软件框架 pipeline，这个pipeline有三个模块：
    // 视频取流 -> 人脸检测 -> 显示模块(QT GUI上显示)
    // 有没有开源的系统架构给参考一下 ？ 最后的显示模块，如何显示到 MainWindow的指定位置？

    return a.exec();
}

