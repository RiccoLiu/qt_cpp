#ifndef OPENCV_DISPLAY_H
#define OPENCV_DISPLAY_H

#include <opencv2/opencv.hpp>
#include <memory>
#include "pipeline_node.h"

class OpenCVDisplay final : public PipelineNode
{
public:
    OpenCVDisplay(const std::string& name);
    virtual ~OpenCVDisplay(){}

    virtual void InitializeThreadOnce();
    virtual void UninitializeThreadOnce();

protected:
    virtual bool LoadSubYaml(YAML::Node& config);
    virtual bool processMsg(std::shared_ptr<PipelineMsg>& msg,std::shared_ptr<PipelineSink>& sink);

private:
    std::string win_name_;

    DISALLOW_COPY_AND_ASSIGN(OpenCVDisplay);
};

/**
 * OpenCVDisplay Creator
 */
std::shared_ptr<PipelineNode> OpenCVDisplayCreator(const std::string& name);

#endif // OPENCV_DISPLAY_H
