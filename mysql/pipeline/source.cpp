#include "source.h"
#include "sink.h"

PipelineSource::PipelineSource(std::shared_ptr<PipelineNode> &node, const std::string &name)
    : name_(name), node_(node)
{
}

bool PipelineSource::RegisterListener(std::shared_ptr<PipelineSink> &listener) {
    listeners_.push_back(listener);
    return true;
}

bool PipelineSource::PublishPipelineMsg(std::shared_ptr<PipelineMsg> &msg) {
    std::list<std::shared_ptr<PipelineSink>>::iterator i = listeners_.begin();
    while (i != listeners_.end()) {
        std::shared_ptr<PipelineSink> &listener = *i;
        if (!listener->OnPipelineMsg(msg, listener))
            return false;
        i++;
    }
    return true;
}
