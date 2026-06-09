#ifndef PIPELINE2_TEST_NODE_FACTORY_H
#define PIPELINE2_TEST_NODE_FACTORY_H

#include "pipeline_node_factory.h"

class NodeFactory final : public PipelineNodeFactory {
public:
    NodeFactory();
    virtual ~NodeFactory();

    virtual std::unique_ptr<PipelineNode> CreateNode(const std::string& node, const std::string& instance) override;

};

#endif // PIPELINE2_TEST_NODE_FACTORY_H
