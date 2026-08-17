#ifndef VIDEO_WORKER_H
#define VIDEO_WORKER_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <opencv2/opencv.hpp>

class VideoWorker : public QObject {
    Q_OBJECT
public:
    explicit VideoWorker(QObject *parent = nullptr);

    bool GetRunning() {
        return is_running_;
    }
    void SetRunning(bool running) {
        is_running_ = running;
    }

public slots:
    void Start(const QString &file);
    void Stop();
    void SetFps(int fps);

private slots:
    void OnTimeout();

signals:
    void ImageReady(QImage image);
    void Finished();
    void Error(const QString &msg);

private:
    QImage to_image(const cv::Mat& mat);
    cv::Mat from_image(const QImage& image);

private:
    bool is_running_;
    int fps_;

    QTimer* timer_;
    cv::VideoCapture cap_;
};


#endif // VIDEO_WORKER_H
