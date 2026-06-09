#ifndef CAMERAFRAME_H
#define CAMERAFRAME_H

#include <opencv2/opencv.hpp>
#include <memory>
#include "pipeline_msg.h"
#include "pipeline_sys_msg.h"

#define OPENCV_FRAME_FOURCC PIPELINE_FOURCC('c','v','f','\0')

/**
 * CameraFrame is the base for all image data.
 */
class OpenCVFrame : public PipelineMsg
{
public:
    OpenCVFrame(int frame_id, cv::Mat image, std::weak_ptr<PipelineSource>& source)
        : PipelineMsg(source)
        , frame_id_(frame_id)
        , image_(image)
    {
        fourcc_ = OPENCV_FRAME_FOURCC;
    }
    virtual ~OpenCVFrame(){}

    uint64_t GetFrameId() {
        return frame_id_;
    }
    cv::Mat GetFrame() {
        return image_;
    }

protected:
    int64_t frame_id_;
    cv::Mat image_;

    // ShareDataManager mShareDatas;

private:
    DISALLOW_COPY_AND_ASSIGN(OpenCVFrame);
};

#endif // CAMERAFRAME_H
