#ifndef SINK_H
#define SINK_H

#include <string>
#include <memory>
#include "pipeline_msg.h"
#include <compiler.h>

class PipelineNode;
class PipelineThread;

/**
 * PipelineSink recieve data from PipelineSource
 * and push the data to PipelineNode.
 */
class PipelineSink  {
public:
    PipelineSink(std::weak_ptr<PipelineNode> &node, const std::string &name);
    virtual ~PipelineSink() {}
    std::weak_ptr<PipelineNode> &GetNode() { return node_; }
    const std::string &GetName() { return name_; }
    std::weak_ptr<PipelineThread> &GetThread() { return thread_; }

    bool OnPipelineMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &self);

private:
    std::string name_;

    std::weak_ptr<PipelineNode> node_;
    std::weak_ptr<PipelineThread> thread_;

    DISALLOW_COPY_AND_ASSIGN(PipelineSink);
};

#endif // SINK_H
