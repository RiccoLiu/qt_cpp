#include "pipeline_msg.h"


void PipelineMsg::GetFourCC(char *fourcc) {
    fourcc[3] = fourcc_ >> 24;
    fourcc[2] = (fourcc_ >> 16) & 0xFF;
    fourcc[1] = (fourcc_ >> 8) & 0xFF;
    fourcc[0] = (fourcc_ >> 0) & 0xFF;
    fourcc[4] = '\0';
}

const char *PipelineMsg::GetName() {
    if (queue_ == nullptr) {
        return nullptr;
    }
    return queue_->GetName();
}
