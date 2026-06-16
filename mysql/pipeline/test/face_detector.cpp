#include <logger2.h>

#include "face_detector.h"
#include "camera_msg.h"

FaceDetector::FaceDetector(const std::string& instance_name)
    : PipelineNode(false, instance_name)
    , model_("")
    , config_file_("")
{
}

FaceDetector::~FaceDetector()
{
}

bool FaceDetector::LoadYAML(const YAML::Node& profile_cfg) {
    YAML::Node private_cfg  = profile_cfg["private"];
    if (!private_cfg.IsNull()) {
        model_ = private_cfg["model"].as<std::string>();
        config_file_ = private_cfg["config_file"].as<std::string>();
    }
    return PipelineNode::LoadYAML(profile_cfg);
}

bool FaceDetector::Initialize() {
    net_ = cv::dnn::readNetFromCaffe(config_file_, model_);
    if (net_.empty()) {
        LOGI("Failed to load DNN face detector model!");
        return false;
    }

    return true;
}

bool FaceDetector::ProcessMsg(MsgPtr msg) {
#if 0
    std::shared_ptr<CameraFrame> frame = std::dynamic_pointer_cast<CameraFrame>(msg);
    if (frame) {
        cv::Mat img = frame->GetImg();

        cv::Mat blob = cv::dnn::blobFromImage(img, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123), false, false);
        net_.setInput(blob);

        cv::Mat detection = net_.forward();
        if (detection.empty() || detection.dims != 4 || detection.size[3] != 7) {
            LOGE("Invalid detection output: dims=%d, size=[%d,%d,%d,%d]",
                 detection.dims,
                 detection.dims>0 ? detection.size[0] : -1,
                 detection.dims>1 ? detection.size[1] : -1,
                 detection.dims>2 ? detection.size[2] : -1,
                 detection.dims>3 ? detection.size[3] : -1);
            return false;
        }

        int numDetections = detection.size[2];
        float* data = (float*)detection.data; // 更安全的方式

        for (int i = 0; i < numDetections; ++i) {
            float confidence = data[i * 7 + 2];
            if (confidence > 0.5) {
                int x1 = static_cast<int>(data[i * 7 + 3] * img.cols);
                int y1 = static_cast<int>(data[i * 7 + 4] * img.rows);
                int x2 = static_cast<int>(data[i * 7 + 5] * img.cols);
                int y2 = static_cast<int>(data[i * 7 + 6] * img.rows);

                // 边界检查
                x1 = std::max(0, std::min(x1, img.cols - 1));
                y1 = std::max(0, std::min(y1, img.rows - 1));
                x2 = std::max(0, std::min(x2, img.cols - 1));
                y2 = std::max(0, std::min(y2, img.rows - 1));

                cv::rectangle(img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);
            }
        }

        Publish(msg);
    }
#endif
    // if (msg->GetFourcc() == PIPELINE_CAMMSG) {
    //     auto cam_msg = std::dynamic_pointer_cast<CameraMsg>(msg);
    //     if (cam_msg) {
    //         cam_msg->GetObject(IMAGE_FRAME_KEY);
    //     }
    //     Publish(msg);
    // }
    Publish(msg);
    return true;
}

void FaceDetector::Cleanup() {
}

