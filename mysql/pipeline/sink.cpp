#include "sink.h"

#include "pipeline_thread.h"
#include "pipeline_node.h"

PipelineSink::PipelineSink(std::weak_ptr<PipelineNode> &node, const std::string &name)
    : name_(name), node_(node) {
    auto promote = node.lock();
    if (promote != nullptr) {
        thread_ = promote->GetThread();
    }
}

bool PipelineSink::OnPipelineMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &self) {
    auto promote = thread_.lock();
    if (promote == nullptr) {
        return false;
    }
    return promote->OnPipelineMsg(msg, self);
}
