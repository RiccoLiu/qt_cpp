
#include <chrono>
#include <thread>
#include <logger2.h>

#include "pipeline_node.h"

PipelineNode::PipelineNode(bool is_source, const std::string& instance_name)
    : instance_name_(instance_name), is_source_(is_source), queue_max_size_(5) {}

PipelineNode::~PipelineNode() { Stop(); }

bool PipelineNode::LoadYAML(const YAML::Node& profile_cfg) {
    if (profile_cfg.IsNull()) {
        return false;
    }
    for (const auto& topic : profile_cfg["publish"]) {
        std::string topic_name = topic.as<std::string>();
        publish_topic_.push_back(topic_name);
    }
    for (const auto& topic : profile_cfg["subscribe"]) {
        std::string topic_name = topic.as<std::string>();
        Subscribe(topic_name);
        subscribe_topic_.push_back(topic_name);
    }
    return true;
}

void PipelineNode::Start() {
    running_ = true;
    worker_thread_ = std::thread(&PipelineNode::ThreadLoop, this);
}

void PipelineNode::Stop() {
    running_ = false;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void PipelineNode::Subscribe(const std::string& topic) {
    MessageBroker::GetInstance().Subscribe(topic, [this](MsgPtr msg) -> int {
        return this->PushMsg(msg);
    });
}

void PipelineNode::Publish(const std::string& topic, MsgPtr msg) {
    MessageBroker::GetInstance().Publish(topic, msg);
}

void PipelineNode::Publish(MsgPtr msg) {
    for (const auto& topic : publish_topic_) {
        Publish(topic, msg);
    }
}

int PipelineNode::PushMsg(MsgPtr msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.size() > queue_max_size_) {
        LOGW("--- %s drop a msg, queue size = %lu", GetNodeName().c_str(), queue_.size());
        queue_.pop();
    }
    queue_.push(msg);
    if (!queue_.empty()) {
        condition_.notify_one();
    }
    return queue_.size();
}

MsgPtr PipelineNode::PopMsg() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        auto status = condition_.wait_for(lock, std::chrono::milliseconds(30));
        if (status == std::cv_status::timeout) {
            return nullptr;
        }
    }
    auto msg = queue_.front();
    queue_.pop();
    return msg;
}

void PipelineNode::ThreadLoop() {
    Initialize();
    while (running_) {
        if (is_source_) {
            Process();
        } else {
            MsgPtr msg = PopMsg();
            if (msg) {
                ProcessMsg(msg);
            }
        }
    }
    Cleanup();
}
