#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include "pipeline_node.h"
#include <opencv2/opencv.hpp>

class FaceDetector final : public PipelineNode {
public:
    FaceDetector(const std::string& name);
    virtual ~FaceDetector();

    virtual void InitializeThreadOnce() override;
    virtual void UninitializeThreadOnce() override;

    virtual bool LoadSubYaml(YAML::Node &config) override;
    virtual bool processMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink) override;

private:
    cv::dnn::Net m_net;
    std::string model_;
    std::string config_file_;

    std::shared_ptr<PipelineSource> source_;

    bool m_valid_marker = true;
};

std::shared_ptr<PipelineNode> CreateFaceDetector(const std::string& name);

#endif // FACE_DETECTOR_H
