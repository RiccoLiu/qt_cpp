#ifndef PIPELINE2_MESSAGE_BROKEN_H
#define PIPELINE2_MESSAGE_BROKEN_H

#include "pipeline_msg.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <functional>

using SubscriberCallback = std::function<int(MsgPtr)>;

class MessageBroker {
public:
    static MessageBroker& GetInstance();
    void Subscribe(const std::string& topic, SubscriberCallback cb);
    void Publish(const std::string& topic, MsgPtr msg);

private:
    MessageBroker() = default;
    std::unordered_map<std::string, std::vector<SubscriberCallback>> subscribers_;
    std::mutex mutex_;
};
#endif // PIPELINE2_MESSAGE_BROKEN_H
