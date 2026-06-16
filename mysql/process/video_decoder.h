#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <yaml-cpp/yaml.h>
#include <pipeline_node.h>
#include <opencv2/opencv.hpp>

class VideoDecoder : public PipelineNode {
public:
    VideoDecoder(const std::string& instance_name);

    virtual bool Process();
    virtual bool ProcessMsg(MsgPtr msg) {
        return false;
    }

    virtual std::string GetNodeName() {
        return VideoDecoder::NodeName();
    }
    static std::string NodeName() {
        return "VideoDecoder";
    }

public:
    bool Open(const std::string& video_file);
    void Close();

    bool IsPause() {
        return is_pause_;
    }
    void SetPause(bool pause) {
        is_pause_ = pause;
    }

private:
    std::atomic<bool> is_pause_ {true};

    uint64_t frame_id_ {0};
    int width_ {0};
    int height_ {0};
    int fps_ {0};

    std::string file_;
    cv::VideoCapture cap_;
};

#endif // VIDEO_DECODER_H
