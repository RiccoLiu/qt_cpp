#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <opencv2/opencv.hpp>

namespace Utils {

inline QImage cvMatToQImage(const cv::Mat& mat) {
    if (mat.empty()) return QImage();
    cv::Mat rgb;
    if (mat.channels() == 3) {
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    } else if (mat.channels() == 1) {
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    } else {
        rgb = mat.clone();
    }
    return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
}

}

#endif // UTILS_H
