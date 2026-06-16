#include <logger2.h>
#include <image.h>
#include <image_pyramid.h>

#include "camera_msg.h"
#include "video_decoder.h"

VideoDecoder::VideoDecoder(const std::string& instance_name)
    : PipelineNode(true, instance_name)
{
}

bool VideoDecoder::Process() {
    if (IsPause()) {
        return true;
    }

    std::shared_ptr<utils::ImageFrame> img = utils::ImageFrameAccquire<utils::RGBColor>(width_, height_);

    cv::Mat cv_img;
    img->ConvertTo(cv_img);

    if (cap_.read(cv_img) && !cv_img.empty()) {
        // 原始图像
        auto cam_msg = PipelineMsgPool<CameraMsg>::GetInstance().AcquireData();
        cam_msg->SetFrameId(frame_id_);
        cam_msg->AddObject(IMAGE_FRAME_KEY, img);

        // 灰度图像
        auto sharedata = cam_msg->GetObject(GRAY_FRAME_KEY);
        if (sharedata == nullptr) {
            std::shared_ptr<utils::ImageFrame> gray = utils::ImageFrameAccquire<unsigned char>(width_, height_);

            cv::Mat cv_gray;
            gray->ConvertTo(cv_gray);

            cv::cvtColor(cv_img, cv_gray, cv::COLOR_BGR2GRAY);
            cam_msg->AddObject(GRAY_FRAME_KEY, gray);

            sharedata = cam_msg->GetObject(GRAY_FRAME_KEY);
        }
        std::shared_ptr<utils::ImageFrame> gray = std::dynamic_pointer_cast<utils::ImageFrame>(sharedata);
        if (gray == nullptr) {
            LOGE("Get gray img failed..");
            return false;
        }

        // 灰度金字塔
        sharedata = cam_msg->GetObject(GRAY_PYRAMID_KEY);
        if (sharedata == nullptr) {
            auto gray_pyramid = ShareDataPool<utils::ImagePyramid>::GetInstance().AcquireData();
            if (gray_pyramid) {
                gray_pyramid->Make<unsigned char>(gray);
                cam_msg->AddObject(GRAY_PYRAMID_KEY, gray_pyramid);
            }
        }

        // 图像金字塔
        sharedata = cam_msg->GetObject(IMAGE_PYRAMID_KEY);
        if (sharedata == nullptr) {
            auto pyramid = ShareDataPool<utils::ImagePyramid>::GetInstance().AcquireData();
            if (pyramid) {
                pyramid->Make<utils::RGBColor>(img);
                cam_msg->AddObject(IMAGE_PYRAMID_KEY, pyramid);
            }
        }

        Publish(cam_msg);

        frame_id_++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps_));
    } else {
        // Stop();              // 导致死锁
        // SetRunning(false);   // 退出线程
        Close();
    }
    return true;
}


bool VideoDecoder::Open(const std::string& video_file) {
    file_ = video_file;

    if (!cap_.open(file_)) {
        LOGE("Open %s failed..", file_.c_str());
        return false;
    }

    double width = cap_.get(cv::CAP_PROP_FRAME_WIDTH);
    double height = cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps = cap_.get(cv::CAP_PROP_FPS);

    /*
        cv::CAP_PROP_FRAME_WIDTH	视频帧宽度（像素）
        cv::CAP_PROP_FRAME_HEIGHT	视频帧高度（像素）
        cv::CAP_PROP_FPS	帧率（frames per second）
        cv::CAP_PROP_FRAME_COUNT	总帧数
        cv::CAP_PROP_FORMAT	像素格式（如 CV_8UC3）
    */

    if (width > 0 && height > 0) {
        width_ = static_cast<int>(width);
        height_ = static_cast<int>(height);
        fps_ = static_cast<int>(fps);
        // std::cout << "Resolution: " << width_ << "x" << height_ << std::endl;

    }

    LOGI("Open %s file: Resolution:[%d, %d], fps: %d", file_.c_str(), width_, height_, fps_);
    is_pause_ = false;
    return true;
}

void VideoDecoder::Close() {
    cap_.release();

    frame_id_ = 0;
    width_ = 0;
    height_ = 0;
    fps_ = 0;

    is_pause_ = true;

    LOGI("Close %s file..", file_.c_str());
}

