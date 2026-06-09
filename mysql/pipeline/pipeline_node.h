#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <map>
#include <memory>
#include <string>

#include <singleton.h>
#include <yaml-cpp/yaml.h>

#include "pipeline_msg.h"
#include "sink.h"
#include "source.h"


class Pipeline;
class PipelineThread;

//----------------------------------------------------------------------
struct PipelineNodeMsgItem {
    std::shared_ptr<PipelineMsg> *msg;  ///< temperary items it's ok
    std::shared_ptr<PipelineSink> *sink;
};

#if 0
typedef BlockStackArray<PipelineNodeMsgItem> PipelineNodeMsgItemArray;

namespace common {
template <>
struct traits_name<PipelineNodeMsgItem> {
    static constexpr char *value = (char *)"PipelineNodeMsgItem";
};

BLOCKARRAY_TRAITS(PipelineNodeMsgItem);
}  // namespace common

#endif

/**
 * PipelineNode is main process for a sepecific algorithm.
 */
class PipelineNode : public std::enable_shared_from_this<PipelineNode> {
    friend class PipelineThread;

public:
    PipelineNode(const std::string &name) : cap_queues_(false), name_(name), thread_initialized_(false) {}
    virtual ~PipelineNode() {}

    virtual void InitializeThreadOnce() {}
    virtual void UninitializeThreadOnce() {}

    virtual bool LoadSubYaml(YAML::Node &config) = 0;
    virtual bool processMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink) {return false;}
    virtual bool ProcessMsgArray(std::vector<PipelineNodeMsgItem>& messages) {return false;}

    void registerThreadCleanup();

    const std::string &GetName() {
        return name_;
    }

    std::shared_ptr<PipelineThread> &GetThread() {
        return mThread;
    }

    bool LoadYAML(YAML::Node &config);

    bool IsDumb() {
        return sinks_.empty();
    }

    bool ProcessMsgs(std::vector<PipelineNodeMsgItem> &messages) {
        if (!thread_initialized_) {
            InitializeThreadOnce();
            thread_initialized_ = true;
        }

        bool ret = true;
        if (cap_queues_)
            ret = ProcessMsgArray(messages);
        else {
            std::vector<PipelineNodeMsgItem>::iterator iter = messages.begin();
            for (; iter != messages.end(); ++iter) {
                bool ok = processMsg(*(iter->msg), *(iter->sink));
                if (!ok) {
                    printf("%s::ProcessMsg failed!!!!!", GetName().c_str());
                }
                ret &= ok;
            }
        }
        return ret;
    }

    std::shared_ptr<PipelineSink> GetSink(const std::string &name) {
        std::map<std::string, std::shared_ptr<PipelineSink>>::iterator i = sinks_.find(name);
        if (i != sinks_.end())
            return i->second;
        return nullptr;
    }
    std::shared_ptr<PipelineSource> GetSource(const std::string &name) {
        std::map<std::string, std::shared_ptr<PipelineSource>>::iterator i = sources_.find(name);
        if (i != sources_.end())
            return i->second;
        return nullptr;
    }

    void PublishMsg(std::shared_ptr<PipelineMsg> msg) {
        for (auto [name, source] : sources_) {
            source->PublishPipelineMsg(msg);
        }
    }

    void enableCapQueues() {
        cap_queues_ = true;
    }
    void disableCapQueues() {
        cap_queues_ = false;
    }

protected:
    bool cap_queues_;
    std::string name_;
    std::weak_ptr<Pipeline> pipeline_;
    std::shared_ptr<PipelineThread> mThread;

    std::map<std::string, std::shared_ptr<PipelineSink>> sinks_;
    std::map<std::string, std::shared_ptr<PipelineSource>> sources_;

private:
    bool thread_initialized_;

    DISALLOW_COPY_AND_ASSIGN(PipelineNode);
};

/**
 * PipelineNodeFactory
 */
typedef std::shared_ptr<PipelineNode> (*NodeCreator)(const std::string &);

class PipelineNodeFactory : public Singleton<PipelineNodeFactory> {
public:
    void RegisterCreator(const std::string &name, NodeCreator creator) {
        node_creators_[name] = creator;
    }

    std::shared_ptr<PipelineNode> Create(const std::string &name, YAML::Node &config) {
        std::string instance_name = name;
        YAML::Node instance = config["instance"];
        if (instance) {
            instance_name += "::";
            instance_name += instance.as<std::string>();
        }

        // LOGD("PipelineNodeFactory Create %s", instance_name.c_str());
        std::map<std::string, NodeCreator>::iterator i = node_creators_.find(name);
        if (i != node_creators_.end()) {
            NodeCreator creator = i->second;
            std::shared_ptr<PipelineNode> node = (*creator)(instance_name);
            if (node != nullptr) {
                YAML::Node profile = config["profile"];
                node->LoadYAML(profile);
            }
            return node;
        }
        return nullptr;
    }

    std::map<std::string, NodeCreator>& GetCreators() {
        return node_creators_;
    }

private:
    std::map<std::string, NodeCreator> node_creators_;
};

#endif // PIPELINE_NODE_H
