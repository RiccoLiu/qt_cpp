#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include "pipeline_node.h""

class VideoDecoder final : public PipelineNode {
public:
    VideoDecoder(const std::string& name);
    virtual ~VideoDecoder();

    virtual void InitializeThreadOnce();
    virtual void UninitializeThreadOnce();

    virtual bool LoadSubYaml(YAML::Node &config);
    virtual bool processMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink);

    int ThreadLoop();

private:
    std::string file_;
    int fps_;

    bool running_;
    std::thread thread_;
};

std::shared_ptr<PipelineNode> OpenCVDecoderCreator(const std::string& name);

#endif // VIDEO_DECODER_H
