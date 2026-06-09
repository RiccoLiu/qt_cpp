#include <logger2.h>
#include "opencv_display.h"
#include "camera_frame.h"

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
    std::shared_ptr<CameraFrame> frame = std::dynamic_pointer_cast<CameraFrame>(msg);
    // LOGI("----OpencvDisplay::ProcessMsg, frame_id: %d ", frame->GetFrameId());
    if (frame) {
        cv::imshow(win_name_, frame->GetImg());
        cv::waitKey(1);
    }
    return true;
}


void OpencvDisplay::Cleanup() {
    cv::destroyWindow(win_name_);
}
