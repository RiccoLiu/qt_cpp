#ifndef IMAGE_CONTROL_H
#define IMAGE_CONTROL_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <opencv2/opencv.hpp>

#include "video_worker.h"

class ImageControl : public QObject
{
    Q_OBJECT
public:
    explicit ImageControl(QObject *parent = nullptr);
    ~ImageControl();

    void SimulateScanLineStart();
    void SimulateScanLineStop();

public slots:
    int Open(QString file);
    void Close();

    void SetGamma(double gamma);
    void SetFps(int fps);

    void SimulateScanLine();

private slots:
    void Publish();

signals:
    void ImageReady(QImage image);
    void Error(const QString& msg);

    void ScanImageReady(QImage image);
    void ScanImageFinished();

private:
    bool IsImageFile(const QString& file);

    QImage ApplyGamma(const QImage& image);
    void RebuildGammaLUT();

    QPixmap to_pixmap(QImage image);
    QImage from_pixmap(QPixmap pixmap);

private:
    double gamma_;
    std::array<quint8, 256> gamma_lut_;

    QImage image_;

    VideoWorker* video_worker_;
    QThread* worker_thread_;

    QTimer* simulate_timer_;
};

#endif // IMAGE_CONTROL_H
