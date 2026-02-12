#include "mainwidget.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>

#include "testcls.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget main_window;

    QString title("水平垂直布局测试");
    main_window.setWindowTitle(title);
    main_window.resize(200, 100);

   int btn_num = 4;

    QVBoxLayout* v_layout = new QVBoxLayout();
    for (int i = 0; i < btn_num; i++) {
        QString name = QString("垂直按钮 %1").arg(i);
        QPushButton* btn = new QPushButton(name);
        v_layout->addWidget(btn);
    }

    QHBoxLayout* h_layout = new QHBoxLayout();
    for (int i = 0; i < btn_num; i++) {
        QString name = QString("水平按钮 %1").arg(i);
        QPushButton* btn = new QPushButton(name);
        h_layout->addWidget(btn);
    }

    QGridLayout* grid_layout = new QGridLayout();
    for (int y = 0; y < btn_num; y++) {
        for (int x = 0; x < btn_num; x++) {// (0, 0), (1, 0)
            QString name = QString("Grid按钮 %1, %2").arg(x, y);
            QPushButton* btn = new QPushButton(name);
            grid_layout->addWidget(btn, x, y);
        }
    }

    // main_window.setLayout(v_layout);
    // main_window.setLayout(h_layout);
    main_window.setLayout(grid_layout);

    TestCls test_cls;
    test_cls.Print();

    main_window.show();
    return a.exec();
}
