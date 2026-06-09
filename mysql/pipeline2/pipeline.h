
#ifndef PIPELINE2_PIPELINE_H
#define PIPELINE2_PIPELINE_H


#include "pipeline_node.h"
#include "pipeline_node_factory.h"
#include <memory>
#include <vector>
#include <string>

class Pipeline {
public:
    Pipeline(std::shared_ptr<PipelineNodeFactory> factory)
        : factory_(factory) {}
    virtual ~Pipeline() {}

    void LoadYAML(const std::string& yaml_file);
    void Start();
    void Stop();

private:
    std::shared_ptr<PipelineNodeFactory> factory_;
    std::vector<std::unique_ptr<PipelineNode>> nodes_;
};

#endif // PIPELINE2_PIPELINE_H

