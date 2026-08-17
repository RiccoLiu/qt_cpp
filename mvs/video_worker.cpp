#include <QThread>
#include <QDebug>

#include "video_worker.h"

VideoWorker::VideoWorker(QObject *parent)
    : QObject{parent}
    , is_running_(false)
    , fps_(30)
    , timer_(new QTimer(this))
{
    // 使用定时器进行非阻塞式采集
    connect(timer_, &QTimer::timeout, this, &VideoWorker::OnTimeout);
}


void VideoWorker::Start(const QString &file) {
    cap_.open(file.toStdString());
    if (!cap_.isOpened()) {
        emit Error("Failed to open video: " + file);
        return;
    }
    is_running_ = true;

    double width = cap_.get(cv::CAP_PROP_FRAME_WIDTH);
    double height = cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
    fps_ = cap_.get(cv::CAP_PROP_FPS);

#if  0
    // 1.循环阻塞式采集会占用线程，导致无法执行其他槽函数，比如Stop无效等
    cv::Mat frame;
    while(is_running_ && cap_.read(frame) && !frame.empty()) {
        // 1. 应用 Gamma
        ApplyGamma(frame);

        // 2. cv::Mat转 QPixmap
        QPixmap pixmap = ConvertCvMatToPixmap(frame);
        emit FrameReady(pixmap);

        // 3. 根据帧率调整休眠时间
        QThread::msleep(50);
    }
#else
    int interval = fps_ > 0 ? (1000 / fps_) : 33;
    timer_->start(interval);
    // qDebug() << "Video started with FPS:" << fps << ", interval:" << interval << "ms";
#endif
}

void VideoWorker::Stop() {
    is_running_ = false;
    timer_->stop();
    cap_.release();
    emit Finished();
}

void VideoWorker::OnTimeout() {
    if (!is_running_)
        return;

    cv::Mat frame;
    if (cap_.read(frame) && !frame.empty()) {
        emit ImageReady(to_image(frame));
    } else {
        Stop();
    }
}

void VideoWorker::SetFps(int fps) {
    qDebug() << "VideoWorker::SetFps : " << fps ;

    fps_ = fps;

    if (is_running_) {
        int interval = fps > 0 ? (1000 / fps) : 33;
        timer_->setInterval(interval);
        if (!timer_->isActive()) {
            timer_->start();
        }
    }
}

QImage VideoWorker::to_image(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }
    switch (mat.type()) {
    case CV_8UC1: {
        // 灰度图
        QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return img.copy();
    }
    case CV_8UC3: {
        // BGR -> RGB
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        QImage img(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        return img.copy();
    }
    case CV_8UC4: {
        // BGRA -> RGBA
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        QImage img(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888);
        return img.copy();
    }
    default:
        qWarning() << "Unsupported cv::Mat type:" << mat.type();
        break;
    }
    return QImage();
}

cv::Mat VideoWorker::from_image(const QImage& image) {
    if (image.isNull()) {
        qWarning() << "Invalid QImage provided.";
        return cv::Mat();
    }

    switch (image.format()) {
    case QImage::Format_Grayscale8: {
        // 灰度图像直接转换
        cv::Mat mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        return mat.clone(); // 返回副本以确保数据独立于原 QImage
    }
    case QImage::Format_RGB888: {
        // RGB 图像转 BGR
        cv::Mat mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat bgr;
        cv::cvtColor(mat, bgr, cv::COLOR_RGB2BGR); // 转换颜色顺序
        return bgr;
    }
    case QImage::Format_RGBA8888: {
        // RGBA 图像转 BGRA
        cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat bgra;
        cv::cvtColor(mat, bgra, cv::COLOR_RGBA2BGRA); // 转换颜色顺序
        return bgra;
    }
    default:
        qWarning() << "Unsupported QImage format:" << image.format();
        break;
    }
    return cv::Mat();
}
