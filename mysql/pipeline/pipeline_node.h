#ifndef PIPELINE2_PIPELINE_NODE_H
#define PIPELINE2_PIPELINE_NODE_H

#include "message_broker.h"
#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <yaml-cpp/yaml.h>

class PipelineNode {
public:
    PipelineNode(bool is_source, const std::string& instance_name);
    virtual ~PipelineNode();

    virtual bool LoadYAML(const YAML::Node& profile_cfg);
    virtual bool Process() = 0;
    virtual bool ProcessMsg(MsgPtr msg) = 0;
    virtual bool Initialize() {
        return true;
    }
    virtual void Cleanup() {
    }

    static std::string NodeName() {
        return "PipelineNode";
    }
    virtual std::string GetNodeName() {
        return PipelineNode::NodeName();
    }

public:
    virtual void Start();
    virtual void Stop();

    virtual void Subscribe(const std::string& topic);
    virtual void Publish(const std::string& topic, MsgPtr msg);
    virtual void Publish(MsgPtr msg);

    int PushMsg(MsgPtr msg);
    MsgPtr PopMsg();

    bool IsRunning() const {
        return running_;
    }
    void SetRunning(bool running) {
        running_ = running;
    }

private:
    void ThreadLoop();

private:
    bool is_source_;
    std::string instance_name_;

    std::vector<std::string> publish_topic_;
    std::vector<std::string> subscribe_topic_;

    std::atomic<bool> running_{false};
    std::thread worker_thread_;

    std::mutex mutex_;
    std::condition_variable condition_;

    int queue_max_size_;
    std::queue<MsgPtr> queue_;
};

#endif // PIPELINE2_PIPELINE_NODE_H
