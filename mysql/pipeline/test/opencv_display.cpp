#include <logger2.h>
#include "opencv_display.h"
#include "camera_msg.h"
#include "image.h"
#include "image_pyramid.h"

OpencvDisplay::OpencvDisplay(const std::string& instance_name)
    : PipelineNode(false, instance_name)
    , win_name_("")
    , fps_(0)
    , win_scale_(1.0)
{
}

OpencvDisplay::~OpencvDisplay()
{
}

bool OpencvDisplay::LoadYAML(const YAML::Node& profile_cfg) {
    YAML::Node private_cfg  = profile_cfg["private"];
    if (!private_cfg.IsNull()) {
        win_name_ = private_cfg["win_name"].as<std::string>();
        fps_ = private_cfg["fps"].as<int>();
        win_scale_ = private_cfg["win_scale"].as<float>();
    }
    return PipelineNode::LoadYAML(profile_cfg);
}

bool OpencvDisplay::Initialize() {
    cv::namedWindow(win_name_, cv::WINDOW_AUTOSIZE);
    return true;
}

bool OpencvDisplay::ProcessMsg(MsgPtr msg) {
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
            cv::imshow("img", cv_img);
            cv::waitKey(1);
        }

        // 显示灰度图像
        auto gray = std::dynamic_pointer_cast<utils::ImageFrame>(cam_msg->GetObject(GRAY_FRAME_KEY));
        if (gray) {
            cv::Mat cv_gray;
            gray->ConvertTo(cv_gray);
            cv::imshow("gray", cv_gray);
            cv::waitKey(1);
        }

        // 显示图像金字塔
        auto img_pyramid = std::dynamic_pointer_cast<utils::ImagePyramid>(cam_msg->GetObject(IMAGE_PYRAMID_KEY));
        if (img_pyramid) {
            for (int i = 0; i < img_pyramid->GetSize(); i++) {
                auto img_i = img_pyramid->GetImage(i);
                if (img_i) {
                    cv::Mat cv_img_i;
                    img_i->ConvertTo(cv_img_i);

                    cv::imshow(std::string("pyramid_").append(std::to_string(i)), cv_img_i);
                    cv::waitKey(1);
                }
            }
        }

        // 显示灰度金字塔
        auto gray_pyramid = std::dynamic_pointer_cast<utils::ImagePyramid>(cam_msg->GetObject(GRAY_PYRAMID_KEY));
        if (gray_pyramid) {
            for (int i = 0; i < gray_pyramid->GetSize(); i++) {
                auto gray_i = gray_pyramid->GetImage(i);
                if (gray_i) {
                    cv::Mat cv_gray_i;
                    gray_i->ConvertTo(cv_gray_i);

                    cv::imshow(std::string("gray_pyramid_").append(std::to_string(i)), cv_gray_i);
                    cv::waitKey(1);
                }
            }
        }
    }

    return true;
}


void OpencvDisplay::Cleanup() {
    cv::destroyWindow(win_name_);
}
