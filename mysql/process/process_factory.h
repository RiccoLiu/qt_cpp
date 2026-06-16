#ifndef PROCESS_FACTORY_H
#define PROCESS_FACTORY_H

#include <pipeline_node_factory.h>

class ProcessFactory : public PipelineNodeFactory {
public:
    ProcessFactory() {}
    virtual ~ProcessFactory() {}

    virtual std::shared_ptr<PipelineNode> CreateNode(const std::string& node_name, const std::string& instance);
};

#endif // PROCESS_FACTORY_H
