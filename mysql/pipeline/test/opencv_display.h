#ifndef PIEPELINE2_TEST_OPENCV_DISPLAY_H
#define PIEPELINE2_TEST_OPENCV_DISPLAY_H


#include <opencv2/opencv.hpp>

#include "pipeline_node.h"

class OpencvDisplay final : public PipelineNode {
public:
    OpencvDisplay(const std::string& instance_name);
    virtual ~OpencvDisplay();

    virtual bool LoadYAML(const YAML::Node& profile_cfg) override;
    virtual bool Initialize() override;
    virtual bool Process() override {
        return false;
    }
    virtual bool ProcessMsg(MsgPtr msg) override;
    virtual void Cleanup() override;

    static std::string NodeName() {
        return "OpencvDisplay";
    }
    virtual std::string GetNodeName() {
        return OpencvDisplay::NodeName();
    }

private:
    std::string win_name_;
    int fps_;
    float win_scale_;
};

#endif // PIEPELINE2_TEST_OPENCV_DISPLAY_H
