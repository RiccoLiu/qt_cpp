#ifndef PIPELINE2_PIPELINE_MSG_H
#define PIPELINE2_PIPELINE_MSG_H

#include <string>
#include <memory>

class PipelineMsg {
public:
    virtual ~PipelineMsg() = default;
    virtual std::string GetTypeName() const = 0;
};

using MsgPtr = std::shared_ptr<PipelineMsg>;

#endif // PIPELINE2_PIPELINE_MSG_H
