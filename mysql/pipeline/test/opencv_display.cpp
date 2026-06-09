
#include <assert.h>
#include <logger2.h>

#include "opencv_display.h"
#include "pipeline_sys_msg.h"
#include "opencv_frame.h"

std::shared_ptr<PipelineNode> OpenCVDisplayCreator(const std::string& name)
{
    std::shared_ptr<PipelineNode> reader = std::make_shared<OpenCVDisplay>(name);
    return reader;
}

OpenCVDisplay::OpenCVDisplay(const std::string& name)
    :PipelineNode(name)
{
}

bool OpenCVDisplay::LoadSubYaml(YAML::Node& config)
{
    YAML::Emitter out;
    out << config;
    LOGI("OpenCVDisplay YAML: \n%s",out.c_str());

    if(config.IsNull()) {
        return false;
    }
    win_name_ = config["WinName"].as<std::string>();

    return true;
}

void OpenCVDisplay::InitializeThreadOnce()
{
    LOGD("OpenCVDisplay InitializeThreadOnce %s",win_name_.c_str());

    cv::namedWindow(win_name_);
}

void OpenCVDisplay::UninitializeThreadOnce()
{
    cv::destroyWindow(win_name_);
}

bool OpenCVDisplay::processMsg(std::shared_ptr<PipelineMsg>& msg,std::shared_ptr<PipelineSink>& sink)
{
    LOGI("---OpenCVDisplay: %s:%d ---->", __func__, __LINE__);

    uint32_t fourcc = msg->GetFourCC();
    if (fourcc == OPENCV_FRAME_FOURCC) {
        std::shared_ptr<OpenCVFrame> camera = std::dynamic_pointer_cast<OpenCVFrame>(msg);
        if (camera) {
            cv::imshow(win_name_, camera->GetFrame());
            cv::waitKey(1);
        }
    }
    return true;
}
