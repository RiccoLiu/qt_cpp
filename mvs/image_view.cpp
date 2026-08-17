#include <QLabel>
#include <QGridLayout>
#include <QPixmap>

#include "image_view.h"

ImageView::ImageView(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(600, 300);

    SetImageViewCount(1);
}

void ImageView::SetImageViewCount(int count) {
    if (count != 1 && count != 4 && count != 9) {
        qWarning() << "Invalid view count:" << count << ", only 1/4/9 allowed.";
        return;
    }
    if (count == img_view_count_) {
        return;
    }

    // 清理旧布局和控件
    QLayout* old_layout = layout();
    if (old_layout) {
        QLayoutItem* item;
        while ((item = old_layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete old_layout;
    }
    displayer_.clear();

    // 计算行列数
    int rows, cols;
    if (count == 1) {
        rows = 1; cols = 1;
    } else if (count == 4) {
        rows = 2; cols = 2;
    } else { // count == 9
        rows = 3; cols = 3;
    }

    // 创建新网格布局
    QGridLayout* new_layout = new QGridLayout(this);
    new_layout->setSpacing(2);
    new_layout->setContentsMargins(2, 2, 2, 2);

    for (int i = 0; i < count; ++i) {
        ImageLabel* label = new ImageLabel(this);
        // label->setText(tr("No Image"));
        // label->setScaledContents(false);            // 自动缩放到控件大小
        // label->setAlignment(Qt::AlignCenter);       // 居中显示
        // label->setBackgroundRole(QPalette::Dark);   // 深色背景
        // label->setAutoFillBackground(true);         // 确保背景色生效
        // label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        displayer_.append(label);
        new_layout->addWidget(label, i / cols, i % cols);
    }

    setLayout(new_layout);
    img_view_count_ = count;

    update();
}

int ImageView::RegisteControl(ImageControl* control) {
    if (control) {
        img_ctl_ = control;

        connect(img_ctl_, &ImageControl::ImageReady, displayer_[0], &ImageLabel::setImage);
        connect(displayer_[0], &ImageLabel::openFile, img_ctl_, &ImageControl::Open);
    }

    return 0;
}



