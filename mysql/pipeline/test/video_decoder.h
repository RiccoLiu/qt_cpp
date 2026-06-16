#ifndef PIPELINE2_TEST_VIDEO_DECODER_H
#define PIPELINE2_TEST_VIDEO_DECODER_H

#include "pipeline_node.h"

#include <opencv2/opencv.hpp>

class VideoDecoder : public PipelineNode {
public:
    VideoDecoder(const std::string& instance);
    virtual ~VideoDecoder() {}

    virtual bool LoadYAML(const YAML::Node& profile_cfg) override;
    virtual bool Initialize() override;
    virtual bool Process() override;
    virtual bool ProcessMsg(MsgPtr msg) override {
        return false;
    };
    virtual void Cleanup() override;


    static std::string NodeName() {
        return "VideoDecoder";
    }
    virtual std::string GetNodeName() {
        return VideoDecoder::NodeName();
    }

private:
    int frame_id_;

    int width_;
    int height_;
    int fps_;

    std::string file_;
    cv::VideoCapture cap_;
};

#endif // PIPELINE2_TEST_VIDEO_DECODER_H
