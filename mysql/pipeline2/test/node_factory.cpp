#include "node_factory.h"

#include "video_decoder.h"
#include "face_detector.h"
#include "opencv_display.h"

NodeFactory::NodeFactory() {}

NodeFactory::~NodeFactory() {}

std::unique_ptr<PipelineNode> NodeFactory::CreateNode(const std::string& node_name, const std::string& instance) {
    std::unique_ptr<PipelineNode> node = nullptr;
    if (node_name == VideoDecoder::NodeName()) {
        node = std::make_unique<VideoDecoder>(instance);
    } else if (node_name == OpencvDisplay::NodeName()) {
        node = std::make_unique<OpencvDisplay>(instance);
    } else if (node_name == FaceDetector::NodeName()) {
        node = std::make_unique<FaceDetector>(instance);
    }
    return node;
}
