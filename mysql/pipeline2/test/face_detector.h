#ifndef PIPELINE2_TEST_FACE_DETECTOR_H
#define PIPELINE2_TEST_FACE_DETECTOR_H

#include <opencv2/opencv.hpp>

#include "pipeline_node.h""

class FaceDetector final : public PipelineNode {
public:
    FaceDetector(const std::string& instance_name);
    virtual ~FaceDetector();

    virtual bool LoadYAML(const YAML::Node& profile_cfg) override;
    virtual bool Initialize() override;
    virtual bool Process() override {
        return false;
    }
    virtual bool ProcessMsg(MsgPtr msg) override;
    virtual void Cleanup() override;

    static std::string NodeName() {
        return "FaceDetector";
    }
    virtual std::string GetNodeName() override {
        return FaceDetector::NodeName();
    }
private:
    cv::dnn::Net net_;
    std::string model_;
    std::string config_file_;

};

#endif // PIPELINE2_TEST_FACE_DETECTOR_H
