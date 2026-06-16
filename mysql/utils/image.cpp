
#include <assert.h>
#include <logger2.h>

#include "image.h"

namespace utils {

/**
 * cv::Mat from ImageFrame without copy
 */
template<>
bool ImageFrame::ConvertTo( cv::Mat& img )
{
    switch(Depth()) {
    case sizeof(unsigned char): {
        cv::Mat mat(Height(), Width(), CV_8UC1, Data());
        img = mat;
        return true;
    }
    case sizeof(float): {
        cv::Mat mat(Height(), Width(), CV_32FC1, Data());
        img = mat;
        return true;
    }
    case sizeof(short): {
        cv::Mat mat(Height(), Width(), CV_16SC1, Data());
        img = mat;
        return true;
    }
    case sizeof(RGBColor): {
        cv::Mat mat(Height(), Width(), CV_8UC3, Data());
        img = mat;
        return true;
    }
    default: {
        LOGD("Convert ImageFrame to cv::Mat, Type:%d", Depth());
        assert(0);
    }}
    return false;
}

/**
 * cv::Mat from ImageFrame with copy
 */
template<>
bool ImageFrame::CopyTo( cv::Mat& img )
{
    cv::Mat mat;
    bool ret = ConvertTo(mat);
    if(ret)
        img = mat.clone();

    return ret;
}

} // namespace utils

