#include "pipeline.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

void Pipeline::LoadYAML(const std::string& yaml_file) {
    YAML::Node config = YAML::LoadFile(yaml_file);
    for (const auto& node_cfg : config["Nodes"]) {
        std::string node_name = node_cfg["node"].as<std::string>();
        std::string instance_name = node_cfg["instance"].as<std::string>();
        auto profile = node_cfg["profile"];

        std::shared_ptr<PipelineNode> node = factory_->CreateNode(node_name, instance_name);
        if (node) {
            if (node->LoadYAML(profile)) {
                nodes_.push_back(node);
            }
        }
    }
}

void Pipeline::Start() {
    for (auto& node : nodes_) {
        node->Start();
    }
}

void Pipeline::Stop() {
    for (auto& node : nodes_) {
        node->Stop();
    }
}
