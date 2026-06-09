
#include <logger2.h>
#include <opencv2/opencv.hpp>

#include "video_decoder.h"
#include "opencv_frame.h"
#include "source.h"

VideoDecoder::VideoDecoder(const std::string& name)
    : PipelineNode(name)
    , running_(false)
{
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::InitializeThreadOnce()
{
    running_ = true;
    thread_ = std::thread(&VideoDecoder::ThreadLoop, this);
}

void VideoDecoder::UninitializeThreadOnce()
{
    running_ = false;
    thread_.join();
}

bool VideoDecoder::LoadSubYaml(YAML::Node &config)
{
    YAML::Emitter out;
    out << config;
    LOGD("logconverter::DataMatch YAML: \n%s", out.c_str());

    if (config.IsNull()) {
        return false;
    }

    file_ = config["file"].as<std::string>();
    fps_ = config["fps"].as<int>();
    return true;
}

bool VideoDecoder::processMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink) {
    return true;
}

int VideoDecoder::ThreadLoop() {
    std::shared_ptr<PipelineSource> source = GetSource("OpenCVFrame");
    if (!source) {
        LOGE("can not find OpenCVFrame source..");
        return -1;
    }

    cv::VideoCapture cap;
    cap.open(file_);
    if (!cap.isOpened()) {
        LOGE("open file %s failed..", file_.c_str());
        return -1;
    }

    int frame_id = 0;
    cv::Mat frame;


    while (running_ && cap.read(frame)) {
        if (frame.empty())
            break;

        std::weak_ptr<PipelineSource> wsource = source;
        std::shared_ptr<OpenCVFrame> img = std::make_shared<OpenCVFrame>(frame_id, frame, wsource);
        std::shared_ptr<PipelineMsg> msg =img;

        // PublishMsg(img);
        // if (source) {
        //     source->PublishPipelineMsg(msg);
        // }

        PublishMsg(msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        frame_id++;
    }
    cap.release();
    return 0;
}

std::shared_ptr<PipelineNode> OpenCVDecoderCreator(const std::string& name)
{
    return std::make_shared<VideoDecoder>(name);
}

