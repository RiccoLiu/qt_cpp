#include <iostream>
#include <logger2.h>

#include "pipeline_node.h"
#include "pipeline_thread.h"
#include "source.h"

/**
 * Functions Implement Section
 */

bool PipelineNode::LoadYAML(YAML::Node &config) {
    std::shared_ptr<PipelineNode> spthis = shared_from_this();
    std::weak_ptr<PipelineNode> wpthis = shared_from_this();

    YAML::Emitter out;
    out << config;
    LOGD("Node YAML: \n%s", out.c_str());

    std::string name = "main";
    YAML::Node thread = config["thread"];
    if (thread)
        name = thread.as<std::string>();
    mThread = PipelineThreadManager::GetInstance().GetThread(name);

    YAML::Node sources = config["Sources"];
    for (std::size_t i = 0; i < sources.size(); i++) {
        name = sources[i].as<std::string>();
        sources_[name] = std::make_shared<PipelineSource>(spthis, name);
    }

    YAML::Node sinks = config["Sinks"];
    for (std::size_t i = 0; i < sinks.size(); i++) {
        name = sinks[i].as<std::string>();
        sinks_[name] = std::make_shared<PipelineSink>(wpthis, name);
    }

    YAML::Node subconfig = config["private"];
    if (subconfig.Type() == YAML::NodeType::Scalar) {
        std::string path = subconfig.as<std::string>();
        subconfig = YAML::LoadFile(path);
    }
    return LoadSubYaml(subconfig);
}

// 线程局部清理器
class ThreadLocalCleanup {
public:
    ~ThreadLocalCleanup() {
        std::cout << "Thread cleanup: calling UninitializeThreadOnce for "
                  << nodes_.size() << " nodes\n";
        for (auto& node : nodes_) {
            if (node) {
                node->UninitializeThreadOnce();
            }
        }
    }

    void add(std::shared_ptr<PipelineNode> node) {
        nodes_.push_back(std::move(node));
    }

private:
    std::list<std::shared_ptr<PipelineNode>> nodes_;
};

// 每个线程独立的清理器实例
thread_local static ThreadLocalCleanup g_thread_cleanup;

void PipelineNode::registerThreadCleanup() {
    auto thisnode = PipelineNode::shared_from_this();
    g_thread_cleanup.add(thisnode);
}

