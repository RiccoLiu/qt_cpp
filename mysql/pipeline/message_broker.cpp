
#include "message_broker.h"

MessageBroker& MessageBroker::GetInstance() {
    static MessageBroker instance;
    return instance;
}

void MessageBroker::Subscribe(const std::string& topic, SubscriberCallback cb) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_[topic].push_back(cb);
}

void MessageBroker::Publish(const std::string& topic, MsgPtr msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = subscribers_.find(topic);
    if (it != subscribers_.end()) {
        for (auto& cb : it->second) {
            cb(msg);
        }
    }
}
