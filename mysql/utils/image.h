
#ifndef UTILS_IMAGE_H
#define UTILS_IMAGE_H

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <assert.h>
#include <opencv2/opencv.hpp>

#include "image_container.h"
#include "sharedata.h"
#include "pixel_type.h"
#include "trait_format.h"

namespace utils {

/**
 * ImageFrame:
 *      兼容 Opencv: BGR 存储
 */
class ImageFrame : public ShareData {
public:
    ImageFrame()  {}
    virtual ~ImageFrame() {}

    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual int Padding() = 0;
    virtual size_t Depth() = 0;
    virtual void* Data() = 0;
    virtual void Resize(int height,int padding)  = 0;
    virtual uint64_t Format() = 0;
    virtual void* Image() = 0;

    // 转换 Opencv 接口
    template<typename T> bool ConvertTo(T& img);
    template<typename T> bool CopyTo(T& img);

    static const char* ClassName() {
        return "ImageFrame";
    }
    virtual const char* GetClassName() {
        return ImageFrame::ClassName();
    }
};

template<> bool ImageFrame::ConvertTo(cv::Mat& img);
template<> bool ImageFrame::CopyTo(cv::Mat& img);

/**
 * ImageFrameT:
 *      ImageFrameT<unsigned char>
 *      ImageFrameT<RGB>
 */
template<typename PixelT>
class ImageFrameT : public ImageFrame {
public:
    ImageFrameT(int width, int height, int padding = 0)
        : depth_(sizeof(PixelT)), image_(width, height, padding, false) {}
    virtual ~ImageFrameT() {}

    virtual int Width() override {
        return image_.Width();
    }
    virtual int Height() override {
        return image_.Height();
    }
    virtual int Padding() override {
        return image_.Padding();
    }
    virtual size_t Depth() override {
        return depth_;
    }
    virtual void* Data() override {
        return image_.Data();
    }
    virtual void Resize(int height, int padding) override {
        image_.ResetV(height,padding);
    }
    virtual uint64_t Format() override {
        return utils::traits_format<PixelT>::value;
    }
    virtual void* Image() override {
        return &image_;
    }

    static const char* ClassName() {
        return "ImageFrameT";
    }
    virtual const char* GetClassName() {
        return ImageFrameT::ClassName();
    }

private:
    size_t depth_;
    utils::Image<PixelT> image_;
};

/**
 * ImageFrameFactory:
 */
template<typename ImageFrameT>
class ImageFrameFactory  {
public:
    using ImageArray = std::vector<std::shared_ptr<ImageFrame>>;
    using ImageArrayMap = std::map<uint64_t, ImageArray>;

public:
    static ImageFrameFactory& GetInstance() {
        static ImageFrameFactory<ImageFrameT> instance;
        return instance;
    }

    std::shared_ptr<ImageFrame> AccquireImage(int width, int height,int padding = 0) {
        std::lock_guard<std::mutex> lock(mutex_);

        ++accquire_cnt_;

        std::shared_ptr<ImageFrame> data = nullptr;

        uint64_t key = get_key(width, height, padding);
        if (auto it = free_array_.find(key); it != free_array_.end()) {
            ImageArray& array = it->second;
            if (!array.empty()) {
                data = array.back();
                array.pop_back();
            }
        }

        if (!data) {
            data = std::make_shared<ImageFrameT>(width, height, padding);
            allocated_++;
            miss_cnt_ ++;
        } else {
            data->Resize(height, padding);
            hit_cnt_++;
        }

        if(miss_cnt_ >= max_free_array_size_ && miss_cnt_ > accquire_cnt_ / 2) {
            LOGW("ImageFrame missing count is too high! AccquireCnt:%d, MissCnt:%d, MissingRate:%.2f!",
                accquire_cnt_, miss_cnt_, 1.0f*miss_cnt_/accquire_cnt_);
        }

        // 创建带自定义删除器的控制块
        auto recycler = std::shared_ptr<void>(
            nullptr,
            [this, data](void*) {
                this->ReleaseData(data);
            }
            );
        return std::shared_ptr<ImageFrame>(recycler, data.get());
    }

    void ReleaseData(std::shared_ptr<ImageFrame> image) {
        std::lock_guard<std::mutex> lock(mutex_);

        uint64_t key = get_key(image->Width(), image->Height(), image->Padding());

        if (auto it = free_array_.find(key); it != free_array_.end()) {
            it->second.push_back(image);
        } else {
            ImageArray array;
            array.push_back(image);
            free_array_[key] = array;
        }

        ImageArrayMap::iterator it = free_array_.find(key);
        ImageArray& array = it->second;

        //判断free list是否达到规定的上限
        if(static_cast<int>(array.size()) > max_free_array_size_) {
            array.pop_back();
        }
    }

    int GetAllocated() const {
        return allocated_;
    }
    int GetFree() const {
        return free_array_.size();
    }

    uint64_t get_key(int width, int height, int padding) {
        return (((uint64_t)width) << 32) | ((uint64_t)(height + padding));
    }

private:
    ImageFrameFactory() = default;
    ~ImageFrameFactory() = default;

private:
    std::mutex mutex_;

    uint64_t allocated_ {0};
    uint64_t accquire_cnt_ {0};
    uint64_t miss_cnt_ {0};
    uint64_t hit_cnt_ {0};
    uint64_t max_free_array_size_ {20};

    ImageArrayMap free_array_;
};

template<typename PixelT>
std::shared_ptr<ImageFrame> ImageFrameAccquire(int w,int h,int padding = 0) {
    return ImageFrameFactory<ImageFrameT<PixelT>>::GetInstance().AccquireImage(w,h,padding);
}

} // namespace utils

#endif /* UTILS_IMAGE_H */
