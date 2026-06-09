#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QObject>

#include <QObject>
#include <opencv2/opencv.hpp>

class VideoDecoder : public QObject {
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);

public slots:
    void startDecoding(const QString& source); // source: "0" for camera, or file path
    void stop();

signals:
    void frameDecoded(const cv::Mat& frame);
    void finished();

private:
    volatile bool m_running = false;
};

#endif // VIDEODECODER_H
