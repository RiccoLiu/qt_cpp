// facedetector.cpp
#include "facedetector.h"
#include <QDir>
#include <QDebug>

FaceDetector::FaceDetector(QObject *parent) : QObject(parent) {
    QString modelPath = QDir::currentPath() + "/res10_300x300_ssd_iter_140000.caffemodel";
    QString configPath = QDir::currentPath() + "/deploy.prototxt";

    m_net = cv::dnn::readNetFromCaffe(configPath.toStdString(), modelPath.toStdString());
    if (m_net.empty()) {
        qWarning() << "Failed to load DNN face detector model!";
    }
}

FaceDetector::~FaceDetector() = default;

void FaceDetector::detect(const cv::Mat& frame) {
    if (frame.empty() || m_net.empty()) {
        emit detectionResult(frame);
        return;
    }

    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123), false, false);
    m_net.setInput(inputBlob);
    cv::Mat detection = m_net.forward();

    cv::Mat result = frame.clone();
    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    for (int i = 0; i < detectionMat.rows; ++i) {
        float confidence = detectionMat.at<float>(i, 2);
        if (confidence > 0.5) {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
            cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);
        }
    }

    emit detectionResult(result);
}
