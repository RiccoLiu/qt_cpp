#include "maindialog.h"

#include <QGridLayout>
#include <iostream>
#include <math.h>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("计算圆的面积"));
    resize(200, 100);

    //
    disp_label_ = new QLabel(this);
    // tr: 支持将字符串转换成多国语言， 建议是将用户可见的文本都用tr来包装，
    disp_label_->setText(tr("请输入圆的半径："));

    input_edit_ = new QLineEdit(this);
    res_label_ = new QLabel(this);
    apply_btn_ = new QPushButton(this);

    apply_btn_->setText(tr("开始计算"));

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(disp_label_, 0, 0);
    layout->addWidget(input_edit_, 0, 1);
    layout->addWidget(res_label_, 1, 0);
    layout->addWidget(apply_btn_, 1, 1);

    // connect(input_edit_, &QLineEdit::textChanged, this, &MainDialog::CalCircleArea);
    connect(apply_btn_, &QPushButton::clicked, this,
                [this] () {
                    this->CalCircleArea();
                }
            );

}

MainDialog::~MainDialog() {}


void MainDialog::CalCircleArea() {
    std::cout << "----" << __func__ << ":" << __LINE__ << "----" << std::endl;

    double radius = input_edit_->text().toDouble();
    double area = M_PI * radius * radius;

    QString res;
    res.setNum(area);
    res_label_->setText(res);
}
