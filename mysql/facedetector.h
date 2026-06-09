// facedetector.h
#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>

class FaceDetector : public QObject {
    Q_OBJECT
public:
    explicit FaceDetector(QObject *parent = nullptr);
    ~FaceDetector();

public slots:
    void detect(const cv::Mat& frame);

signals:
    void detectionResult(const cv::Mat& result);

private:
    cv::dnn::Net m_net;
};
