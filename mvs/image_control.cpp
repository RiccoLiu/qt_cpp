
#include <QFileInfo>
#include <QTimer>
#include <QImageReader>
#include "image_control.h"

ImageControl::ImageControl(QObject *parent)
    : QObject{parent}
    , gamma_(1.0)
    , video_worker_(nullptr)
    , worker_thread_(nullptr)
    , simulate_timer_(new QTimer(this))
{
    RebuildGammaLUT();

    connect(simulate_timer_, &QTimer::timeout, this, &ImageControl::SimulateScanLine);
}

ImageControl::~ImageControl() {
    Close(); // 确保资源释放
}

int ImageControl::Open(QString file) {
    Close();

    // 图片文件
    if (IsImageFile(file)) {
        image_ = QImage(file);
        Publish();
        return 0;
    }

    // 视频文件
    video_worker_ = new VideoWorker();
    worker_thread_ = new QThread();

    video_worker_->moveToThread(worker_thread_);

    connect(video_worker_, &VideoWorker::ImageReady, this, [this] (QImage image) { image_ = image; Publish(); }, Qt::QueuedConnection);
    connect(video_worker_, &VideoWorker::Finished, this, [this] () { Close(); });
    connect(video_worker_, &VideoWorker::Error, this, &ImageControl::Error);

    connect(worker_thread_, &QThread::finished, video_worker_, &QObject::deleteLater);
    connect(worker_thread_, &QThread::finished, worker_thread_, &QThread::deleteLater);

    worker_thread_->start();

    // 开始取流
    QMetaObject::invokeMethod(video_worker_, "Start", Q_ARG(QString, file));

    return 0;
}

void ImageControl::Close() {
    if (video_worker_) {
        QMetaObject::invokeMethod(video_worker_, "Stop");
    }

    if (worker_thread_ && worker_thread_->isRunning()) {
        worker_thread_->quit();
        worker_thread_->wait(); // 等待退出
        worker_thread_ = nullptr;   // 避免重复 delete
    }

    video_worker_ = nullptr;
    worker_thread_ = nullptr;

    qDebug() << "Closed media";
}

void ImageControl::Publish() {
    emit ImageReady(ApplyGamma(image_));
}

void ImageControl::SetGamma(double gamma) {
    gamma_ = gamma;
    RebuildGammaLUT();
    Publish();
}

void ImageControl::SetFps(int fps) {
    if (video_worker_) {
        QMetaObject::invokeMethod(video_worker_, "SetFps", Q_ARG(int, fps));
    }
}

bool ImageControl::IsImageFile(const QString& filePath) {
    QFileInfo info(filePath);
    if (!info.exists() || info.isDir())
        return false;
    QString suffix = info.suffix().toLower();
    QList<QByteArray> supported = QImageReader::supportedImageFormats();
    for (const QByteArray& fmt : supported) {
        if (suffix == QString::fromLatin1(fmt)) { //
            return true;
        }
    }
    return false;
}

void ImageControl::RebuildGammaLUT() {
    if (gamma_ <= 0.0) {
        for (int i = 0; i < 256; ++i) {
            gamma_lut_[i] = static_cast<quint8>(i);
        }
        return;
    }

    const double inv_gamma = 1.0 / gamma_;
    for (int i = 0; i < 256; ++i) {
        double normalized = i / 255.0;
        double corrected = std::pow(normalized, inv_gamma);
        gamma_lut_[i] = static_cast<quint8>(std::round(corrected * 255.0));
    }
}

QImage ImageControl::ApplyGamma(const QImage& image) {
    if (image.isNull()) {
        return QImage();
    }

    QImage img2 = image.convertToFormat(image.hasAlphaChannel()
                                            ? QImage::Format_RGBA8888
                                            : QImage::Format_RGB888);

    const quint8* lut = gamma_lut_.data();

    if (img2.format() == QImage::Format_RGB888) {
        for (int y = 0; y < img2.height(); ++y) {
            uchar* line = img2.scanLine(y);
            for (int x = 0; x < img2.width(); ++x) {
                int i = x * 3;
                line[i]     = lut[line[i]];     // R
                line[i + 1] = lut[line[i + 1]]; // G
                line[i + 2] = lut[line[i + 2]]; // B
            }
        }
    } else if (img2.format() == QImage::Format_RGBA8888) {
        for (int y = 0; y < img2.height(); ++y) {
            uchar* line = img2.scanLine(y);
            for (int x = 0; x < img2.width(); ++x) {
                int i = x * 4;
                line[i]     = lut[line[i]];     // R
                line[i + 1] = lut[line[i + 1]]; // G
                line[i + 2] = lut[line[i + 2]]; // B
                // A 不变
            }
        }
    }
    return img2;
}

QPixmap ImageControl::to_pixmap(QImage image) {
    return QPixmap::fromImage(image);
}

QImage ImageControl::from_pixmap(QPixmap pixmap) {
    return pixmap.toImage();
}

void ImageControl::SimulateScanLineStart() {
    if (simulate_timer_) {
        simulate_timer_->start(10);
    }
}

void ImageControl::SimulateScanLineStop() {
    if (simulate_timer_) {
        simulate_timer_->stop();

        emit ScanImageFinished();
    }
}

void ImageControl::SimulateScanLine() {
    QImage scanLine(2, 960, QImage::Format_Grayscale16);
    for (int y = 0; y < scanLine.height(); ++y) {
        uint16_t *row = reinterpret_cast<uint16_t*>(scanLine.scanLine(y));
        for (int w = 0; w < scanLine.width(); w++) {
            row[w] = rand() % 65535;
        }
    }
    emit ScanImageReady(scanLine);
}

