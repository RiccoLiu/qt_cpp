
#include <opencv2/opencv.hpp>

#include <image.h>

#include "camera_msg.h"
#include "video_displayer.h"

VideoDisplayer::VideoDisplayer(const std::string& instance_name, QObject *parent)
    : QObject{parent}
    , PipelineNode(false, instance_name)
{}

bool VideoDisplayer::ProcessMsg(MsgPtr msg) {
    if (msg->GetFourcc() == PIPELINE_CAMMSG) {
        auto cam_msg = std::dynamic_pointer_cast<CameraMsg>(msg);
        if (!cam_msg) {
            LOGE("Get cam msg failed..");
            return false;
        }

        // 显示原始图像
        auto img = std::dynamic_pointer_cast<utils::ImageFrame>(cam_msg->GetObject(IMAGE_FRAME_KEY));
        if (img) {
            cv::Mat cv_img;
            img->ConvertTo(cv_img);

            cv::Mat rgb;
            if (cv_img.channels() == 3) {
                cv::cvtColor(cv_img, rgb, cv::COLOR_BGR2RGB);
            } else if (cv_img.channels() == 1) {
                cv::cvtColor(cv_img, rgb, cv::COLOR_GRAY2RGB);
            } else {
                rgb = cv_img.clone();
            }
            QImage q_img(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
            QImage disp_img = q_img.copy();
            emit Display(disp_img);
        }
    }
    return true;
}
