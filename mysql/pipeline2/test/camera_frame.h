#ifndef PIEPELINE2_TEST_CAMERA_FRAME_H
#define PIEPELINE2_TEST_CAMERA_FRAME_H

#include <opencv2/opencv.hpp>
#include "pipeline_msg.h"

class CameraFrame : public PipelineMsg {
public:
    CameraFrame(uint64_t frame_id, cv::Mat img)
        : frame_id_(frame_id), img_(img) {}
    virtual ~CameraFrame() {}

    virtual std::string GetTypeName() const {
        return "CameraFrame";
    }
    virtual int GetFrameId() {
        return frame_id_;
    }
    virtual cv::Mat GetImg() {
        return img_;
    }

private:
    int frame_id_;
    cv::Mat img_;
};

#endif // PIEPELINE2_TEST_CAMERA_FRAME_H
