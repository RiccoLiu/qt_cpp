#ifndef SOURCE_H
#define SOURCE_H

#include <list>
#include <string>
#include <memory>
#include <compiler.h>

#include "pipeline_msg.h"

class PipelineNode;
class PipelineSink;

/**
 * PipelineSink recieve data from PipelineSource
 * and push the data to PipelineNode.
 */
class PipelineSource {
public:
    PipelineSource(std::shared_ptr<PipelineNode> &node, const std::string &name);
    virtual ~PipelineSource() {}

    bool RegisterListener(std::shared_ptr<PipelineSink> &listener);
    bool PublishPipelineMsg(std::shared_ptr<PipelineMsg> &msg);

    const std::string &GetName() {
        return name_;
    }
    std::weak_ptr<PipelineNode> GetNode() {
        return node_;
    }
    int NumSinks() {
        return listeners_.size();
    }

private:
    std::string name_;
    std::weak_ptr<PipelineNode> node_;

    std::list<std::shared_ptr<PipelineSink>> listeners_;

    DISALLOW_COPY_AND_ASSIGN(PipelineSource);
};
#endif // SOURCE_H
