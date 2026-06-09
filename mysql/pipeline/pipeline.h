#ifndef PIPELINE_H
#define PIPELINE_H

#include <map>
#include <string>
#include <list>
#include <memory>

#include "pipeline_node.h"
#include "pipeline_sys_msg.h"

/**
 * Pipeline: framework for data process module.
 */
class Pipeline  {
public:
    Pipeline(const std::string &name);
    virtual ~Pipeline() {}

    const std::string &GetName() {
        return name_;
    }

    std::shared_ptr<PipelineNode> GetNode(const std::string &name) {
        std::map<std::string, std::shared_ptr<PipelineNode>>::iterator i = nodes_.find(name);
        if (i != nodes_.end()) {
            return i->second;
        }
        return nullptr;
    }

    bool LoadYAML(YAML::Node &config);

    bool Start();
    bool Stop();
    bool Main();  ///< switch to main thread loop
    void test();

private:
    std::string name_;

    std::map<std::string, std::shared_ptr<PipelineNode>> nodes_;
    std::list<std::shared_ptr<PipelineNode>> startup_nodes_;

    std::shared_ptr<PipelineThread> main_thread_;
    std::shared_ptr<PipelineMsgQueue<PipelineSysMsg>> msg_queue_;

    DISALLOW_COPY_AND_ASSIGN(Pipeline);
};


#endif // PIPELINE_H
