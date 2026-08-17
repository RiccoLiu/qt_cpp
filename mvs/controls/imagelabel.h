#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QWidget>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QToolTip>

// 继承控件： 增强原始控件的功能
class ImageLabel : public QLabel {
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = nullptr)
        : QLabel(parent), scale_ratio_(1.0) {
        setText("Drag & Drop Image Here");

        setScaledContents(false);           // 自动缩放到控件大小
        setAlignment(Qt::AlignCenter);      // 居中显示
        // setBackgroundRole(QPalette::Dark);  // 深色背景
        // setAutoFillBackground(true);        // 确保背景色生效
        setStyleSheet("color: gray; border: 2px dashed #ccc;");

        setAcceptDrops(true);               // 允许接收拖拽事件
        setMouseTracking(true);             // 鼠标追踪，无需按下按键即可触发 mouseMoveEvent
    }

signals:
    void openFile(const QString& file);

public slots:
    void setImage(const QImage &image) {
        if (image.isNull()) {
            clearImage();
            return;
        }

        image_ = image;
        updateScaledImage();
    }

    void clearImage() {
        image_ = QImage();
        scale_ratio_ = 1.0;
        setText("Drag & Drop Image Here");
        QToolTip::hideText();
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QLabel::resizeEvent(event);
        if (!image_.isNull()) {
            updateScaledImage();
        }
    }

    // 拖拽进入控件事件
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
            // setStyleSheet("border: 2px solid green; padding: 20px;"); // 视觉反馈
            setStyleSheet("background-color: rgba(0, 255, 0, 30); border: 2px solid green;");
        }
    }
    // 拖拽离开控件事件
    void dragLeaveEvent(QDragLeaveEvent *event) override {
        Q_UNUSED(event);
        // setStyleSheet("");
        setStyleSheet("color: gray; border: 2px dashed #ccc;"); // 恢复默认样式
    }
    // 重写放下事件
    void dropEvent(QDropEvent *event) override {
        setStyleSheet("color: gray; border: 2px dashed #ccc;"); // 恢复默认样式
        if (event->mimeData()->hasUrls()) {
            QString file = event->mimeData()->urls().first().toLocalFile();
            if (!file.isEmpty()) {
                emit openFile(file);
            }
        }
    }
    // 鼠标进入控件事件
    void mouseMoveEvent(QMouseEvent *event) override {
        if (image_.isNull()) {
            QLabel::mouseMoveEvent(event);
            return;
        }

        // 计算图像在 Label 中的实际绘制区域 (基于 AlignCenter 居中显示)
        int label_w = width();
        int label_h = height();
        int ds_image_w = std::round(image_.width() / scale_ratio_);
        int ds_image_h = std::round(image_.height() / scale_ratio_);

        int offset_x = (label_w - ds_image_w) / 2;
        int offset_y = (label_h - ds_image_h) / 2;

        int img_x = std::round((event->pos().x() - offset_x) * scale_ratio_);
        int img_y = std::round((event->pos().y() - offset_y) * scale_ratio_);

        if (img_x >= 0 && img_x < image_.width() && img_y >= 0 && img_y < image_.height()) {
            static QPoint lastPos(-1, -1);
            QPoint curPos(img_x, img_y);

            if (curPos != lastPos) {
                QRgb pix = image_.pixel(img_x, img_y);
                int gray = qGray(pix);

                QString text = QString("Pos:(%1, %2) | Size:(%3, %4) | Gray:%5")
                                   .arg(img_x).arg(img_y)
                                   .arg(image_.width()).arg(image_.height())
                                   .arg(gray);

                QToolTip::showText(event->globalPos(), text, this);

                lastPos = curPos;
            }
        } else {
            QToolTip::hideText(); // 鼠标移出图像区域时, 隐藏 Pos:(x, y) Size: (w, h) Gray: gray

        }
        QLabel::mouseMoveEvent(event);
    }


    void leaveEvent(QEvent *event) override {
        QToolTip::hideText(); // 鼠标离开控件时，隐藏 Pos:(x, y) Size: (w, h) Gray: gray
        QLabel::leaveEvent(event);
    }

private:
    void updateScaledImage() {
        if (image_.isNull() || size().isEmpty()) return;
        QImage scaled = image_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (scaled.width() > 0 && scaled.height() > 0) {
            qreal scale_x = (qreal)image_.width() / (qreal)scaled.width();
            qreal scale_y = (qreal)image_.height() / (qreal)scaled.height();
            scale_ratio_ = qMax(scale_x, scale_y);
        }

        QLabel::setPixmap(QPixmap::fromImage(scaled));
    }

private:
    QImage image_;
    qreal scale_ratio_; // qreal 浮点类型，主要方便跨平台代码
};

#endif // IMAGELABEL_H
