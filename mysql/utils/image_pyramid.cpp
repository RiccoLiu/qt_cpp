
#include <opencv2/opencv.hpp>

#include "image_pyramid.h"

namespace utils {

void CalcPyramidDownAverage(Image<unsigned char>& img_s, Image<unsigned char>& img_d) {
    int width = img_d.Width();
    int height = img_d.Height();
    int padding = (img_s.Width() << 1) - (img_d.Width() << 1);
    unsigned char* s = (unsigned char*)img_s.Data();
    unsigned char* n = s + img_s.Width();
    unsigned char* d = (unsigned char*)img_d.Data();

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            int v = *s++;
            v += (*s++);
            v += (*n++);
            v += (*n++);
            *d++ = (v >> 2);
        }
        s += padding;
        n += padding;
    }
}

template<>
int ImagePyramid::Make<unsigned char>(std::shared_ptr<ImageFrame> image0) {
    if (utils::traits_format<unsigned char>::value != image0->Format()) {
        LOGE("img pyramid make failed..");
        return -1;
    }

    int width = image0->Width();
    int height = image0->Height();

    for (int level = 0; level <= max_level_ ;level++) {
        if (width < top_min_size_ || height < top_min_size_) {
            break;
        }
        if (level == 0) {
            images_.push_back(image0);
        } else {
            width = (width + step_ - 1) / step_;
            height = (height + step_ - 1) / step_;

            std::shared_ptr<ImageFrame> prev_image = images_.back();
            std::shared_ptr<ImageFrame> cur_image = ImageFrameAccquire<unsigned char>(width, height);

            Image<unsigned char>& src = *((Image<unsigned char> *)(prev_image->Image()));
            Image<unsigned char>& dst = *((Image<unsigned char> *)(cur_image->Image()));
            CalcPyramidDownAverage(src, dst);

            images_.push_back(cur_image);
        }
        SetAvailable((uint32_t)(1 << level));
    }
    return 0;
}

template<>
int ImagePyramid::Make<RGBColor>(std::shared_ptr<ImageFrame> image0) {
    if (utils::traits_format<RGBColor>::value != image0->Format()) {
        LOGE("img pyramid make failed..");
        return -1;
    }

    int width = image0->Width();
    int height = image0->Height();

    for (int level = 0; level <= max_level_ ;level++) {
        if (width < top_min_size_ || height < top_min_size_) {
            break;
        }
        if (level == 0) {
            images_.push_back(image0);
        } else {
            width = (width + step_ - 1) / step_;
            height = (height + step_ - 1) / step_;

            std::shared_ptr<ImageFrame> prev_image = images_.back();
            std::shared_ptr<ImageFrame> cur_image = ImageFrameAccquire<RGBColor>(width, height);

            cv::Mat cv_prev_img, cv_cur_img;
            prev_image->ConvertTo(cv_prev_img);
            cur_image->ConvertTo(cv_cur_img);

            cv::resize(cv_prev_img, cv_cur_img, cv::Size(cur_image->Width(), cur_image->Height()));
            images_.push_back(cur_image);
        }
        SetAvailable((uint32_t)(1 << level));
    }
    return 0;
}

} // namespace utils

