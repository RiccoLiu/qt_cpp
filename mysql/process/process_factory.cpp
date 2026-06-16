

#include "process_factory.h"

#include "video_decoder.h"
#include "video_displayer.h"

std::shared_ptr<PipelineNode> ProcessFactory::CreateNode(const std::string& node_name, const std::string& instance) {
    std::shared_ptr<PipelineNode> node = nullptr;
    if (node_name == VideoDecoder::NodeName()) {
        node = std::make_shared<VideoDecoder>(instance);
    } else if (node_name == VideoDisplayer::NodeName()) {
        node = std::make_shared<VideoDisplayer>(instance);
    }
    return node;
}

