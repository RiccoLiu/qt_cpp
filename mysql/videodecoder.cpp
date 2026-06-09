#include "videodecoder.h"

#include "videodecoder.h"
#include <QThread>
#include <QDebug>

VideoDecoder::VideoDecoder(QObject *parent) : QObject(parent) {}

void VideoDecoder::startDecoding(const QString& source) {
    m_running = true;
    cv::VideoCapture cap;
    // int device = source.toInt(&m_running); // try parse as int (camera index)
    // if (m_running) {
    //     cap.open(device);
    // } else {
        cap.open(source.toStdString());
    // }

    if (!cap.isOpened()) {
        qWarning() << "Failed to open video source:" << source;
        emit finished();
        return;
    }

    cv::Mat frame;
    while (m_running && cap.read(frame)) {
        if (frame.empty()) break;
        emit frameDecoded(frame.clone()); // clone to avoid data race
        QThread::msleep(30); // ~30 FPS
    }

    cap.release();
    emit finished();
}

void VideoDecoder::stop() {
    m_running = false;
}
