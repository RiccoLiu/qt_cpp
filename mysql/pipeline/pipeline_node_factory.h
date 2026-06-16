#ifndef PIPELINE2_PIPELINE_NODE_FACTORY_H
#define PIPELINE2_PIPELINE_NODE_FACTORY_H

#include <string>
#include <memory>

#include <yaml-cpp/yaml.h>

#include "pipeline_node.h"

class PipelineNodeFactory {
public:
    PipelineNodeFactory() {}
    virtual ~PipelineNodeFactory() {}

    virtual std::shared_ptr<PipelineNode> CreateNode(const std::string& node_name, const std::string& instance) = 0;
};

#endif // PIPELINE2_PIPELINE_NODE_FACTORY_H
