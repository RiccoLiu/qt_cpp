#include <thread>
#include <chrono>
#include <logger2.h>

#include "video_decoder.h"
#include "camera_frame.h"

VideoDecoder::VideoDecoder(const std::string& instance)
    : PipelineNode(true, instance), file_(""), fps_(0), frame_id_(0) {}

bool VideoDecoder::LoadYAML(const YAML::Node& profile_cfg)
{
    YAML::Node private_cfg  = profile_cfg["private"];
    if (!private_cfg.IsNull()) {
        file_ = private_cfg["file"].as<std::string>();
        fps_ = private_cfg["fps"].as<int>();
    }
    return PipelineNode::LoadYAML(profile_cfg);
}

bool VideoDecoder::Initialize() {
    cap_.open(file_);
    if (!cap_.isOpened()) {
        std::cerr << "Failed to open video: " << file_ << std::endl;
        return false;
    }
    return true;
}

bool VideoDecoder::Process() {
    if (!cap_.isOpened())
        return false;

    cv::Mat frame;
    if (cap_.read(frame) && !frame.empty()) {
        auto msg = std::make_shared<CameraFrame>(frame_id_++, frame);

        // LOGI("---- VideoDecoder:: Process, frame_id: %d", msg->GetFrameId());

        Publish(msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps_));
    } else {
        // Stop(); // 导致死锁
        SetRunning(false);
    }
    return true;
}

void VideoDecoder::Cleanup() {
    cap_.release();
}
