
#ifndef PIPELINE2_PIPELINE_H
#define PIPELINE2_PIPELINE_H

#include <logger2.h>
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

    std::shared_ptr<PipelineNode> GetNode(const std::string& name) {
        std::shared_ptr<PipelineNode> node = nullptr;
        for (auto it : nodes_) {
            LOGI("name = %s, it->GetNodeName = %s", name.c_str(), it->GetNodeName().c_str());
            if (it->GetNodeName() == name) {
                node = it;
                break;
            }
        }
        return node;
    }

private:
    std::shared_ptr<PipelineNodeFactory> factory_;
    std::vector<std::shared_ptr<PipelineNode>> nodes_;
};

#endif // PIPELINE2_PIPELINE_H

