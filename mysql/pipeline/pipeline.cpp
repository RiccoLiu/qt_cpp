
#include <logger2.h>

#include "source.h""
#include "pipeline.h"
#include "pipeline_thread.h"

void Pipeline::test() {
    LOGI("======= Pipeline::test =======");
}

Pipeline::Pipeline(const std::string &name) : name_(name) {
    std::weak_ptr<PipelineSource> source;

    msg_queue_ = std::make_shared<PipelineMsgQueue<PipelineSysMsg>>(source);
    main_thread_ = PipelineThreadManager::GetInstance().GetThread(PIPELINE_THREAD_MAIN);
}

bool Pipeline::LoadYAML(YAML::Node &config) {
    // YAML::Emitter out;
    // out<<config;
    // LOGD("Pipeline YAML:\n %s",out.c_str());

    YAML::Node nodes = config["Nodes"];
    if (!nodes) {
        return false;
    }
    for (std::size_t i = 0; i < nodes.size(); i++) {
        YAML::Node node = nodes[i];
        std::string name = node["node"].as<std::string>();
        std::shared_ptr<PipelineNode> node_instance = PipelineNodeFactory::GetInstance().Create(name, node);
        if (node_instance == nullptr) {
            return false;
        }
        nodes_[node_instance->GetName()] = node_instance;
    }

    YAML::Node edges = config["Edges"];
    if (!edges) {
        return false;
    }

    for (std::size_t i = 0; i < edges.size(); i++) {
        YAML::Node edge = edges[i];
        YAML::Node from = edge["from"];
        YAML::Node to = edge["to"];

        YAML::Emitter out;
        out << from;
        out << to;
        LOGD("PipelineEdges YAML:\n%s", out.c_str());

        std::shared_ptr<PipelineNode> fnode = GetNode(from[0].as<std::string>());
        if (fnode == nullptr) {
            return false;
        }
        std::shared_ptr<PipelineSource> fsource = fnode->GetSource(from[1].as<std::string>());
        std::shared_ptr<PipelineNode> tnode = GetNode(to[0].as<std::string>());
        if (tnode == nullptr) {
            return false;
        }
        std::shared_ptr<PipelineSink> tsink = tnode->GetSink(to[1].as<std::string>());
        fsource->RegisterListener(tsink);
    }

    YAML::Node startups = config["Startups"];
    if (!startups)
        return false;
    for (std::size_t i = 0; i < startups.size(); i++) {
        // YAML::Emitter out;
        // out<<from;
        // out<<to;
        // LOGD("PipelineEdges YAML:\n%s",out.c_str());

        std::shared_ptr<PipelineNode> node = GetNode(startups[i].as<std::string>());
        startup_nodes_.push_back(node);
    }
    return true;
}

bool Pipeline::Start() {
    bool ret = PipelineThreadManager::GetInstance().Start();
    if (ret) {
        if (msg_queue_ == nullptr) {
            return false;
        }
        std::shared_ptr<PipelineSysMsg> sysmsg = msg_queue_->AcquireMessageT();
        std::shared_ptr<PipelineMsg> msg = sysmsg->GetMessage();
        sysmsg->Set(PIPELINE_SYSTEM_STARTUP, nullptr, nullptr);

        std::list<std::shared_ptr<PipelineNode>>::iterator i = startup_nodes_.begin();
        while (i != startup_nodes_.end()) {
            std::shared_ptr<PipelineNode> &node = *i;
            std::shared_ptr<PipelineThread> &thread = node->GetThread();
            std::weak_ptr<PipelineNode> wpnode = node;
            std::shared_ptr<PipelineSink> sink = std::make_shared<PipelineSink>(wpnode, "dumb");
            thread->OnPipelineMsg(msg, sink);

            i++;
        }
    }
    return ret;
}

bool Pipeline::Stop() {
    return PipelineThreadManager::GetInstance().Stop();
}

bool Pipeline::Main() {
    if (main_thread_ == nullptr) {
        return false;
    }
    return main_thread_->run();
}
