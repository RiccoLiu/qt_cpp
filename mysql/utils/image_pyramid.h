#ifndef UTILS_IMAGE_PYRAMID_H
#define UTILS_IMAGE_PYRAMID_H

#include <Eigen/Core>           // Eigen
#include <Eigen/Dense>

#include "image.h"

namespace utils {

class ImagePyramid : public ShareData {
public:
    ImagePyramid() {}
    virtual ~ImagePyramid() {}

    size_t GetTopMinSize() {
        return top_min_size_;
    }
    size_t GetMaxLevel() {
        return max_level_;
    }
    int GetStep() {
        return step_;
    }
    size_t GetSize() {
        return images_.size();
    }
    std::shared_ptr<ImageFrame> GetImage(int i) {
        std::shared_ptr<ImageFrame> img = nullptr;
        if (i >= 0 && i < images_.size()) {
            img = images_[i];
        }
        return img;
    }

    void SetTopMinSize(size_t top_min_size) {
        top_min_size_ = top_min_size;
    }
    void SetMaxLevel(size_t max_level) {
        max_level_ = max_level;
    }
    void SetStep(double step) {
        step_ = step;
    }

    virtual void Reset() override {
        top_min_size_ = 200;
        max_level_ = 3;
        step_ = 2.0;

        images_.clear();
        ShareData::Reset();
    }

    template<typename PixelT>
    int Make(std::shared_ptr<ImageFrame> image0);

    static const char* ClassName() {
        return "ImagePyramid";
    }
    virtual const char* GetClassName() {
        return ImagePyramid::ClassName();
    }

public:
#if 0
    template<typename PixelT>
    int ImagePyramid::Make(std::shared_ptr<ImageFrame> image0) {
        if (utils::traits_format<PixelT>::value != image0->Format()) {
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
                std::shared_ptr<ImageFrame> cur_image = ImageFrameAccquire<PixelT>(width, height);

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
#endif
private:
    size_t top_min_size_ {200};
    size_t max_level_ {3};

    double step_ {2.0};
    std::vector<std::shared_ptr<ImageFrame> > images_;
};

template<> int ImagePyramid::Make<unsigned char>(std::shared_ptr<ImageFrame> image0);
template<> int ImagePyramid::Make<RGBColor>(std::shared_ptr<ImageFrame> image0);

} // namespace utils

#endif // UTILS_IMAGE_PYRAMID_H
