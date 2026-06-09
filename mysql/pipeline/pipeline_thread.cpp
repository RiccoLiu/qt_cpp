#include <assert.h>
#include <logger2.h>

#include "pipeline_thread.h"
#include "pipeline_node.h"
#include "pipeline_msg.h"
#include "sink.h"

std::shared_ptr<PipelineThread> PipelineThreadManager::GetThread(const std::string &name) {
    std::map<std::string, std::shared_ptr<PipelineThread>>::iterator i = thread_map_.find(name);
    if (i == thread_map_.end()) {
        std::shared_ptr<PipelineThread> thread = std::make_shared<PipelineThread>(name);
        thread_map_[name] = thread;

        LOGD("Thread %s created!!!!!", name.c_str());
        return thread;
    }

    return i->second;
}

bool PipelineThreadManager::Start() {
    std::map<std::string, std::shared_ptr<PipelineThread>>::iterator i = thread_map_.begin();
    while (i != thread_map_.end()) {
        if (0 != (i->first.compare(PIPELINE_THREAD_MAIN))) {
            std::shared_ptr<PipelineThread> thread = i->second;
            LOGD("Thread %s run!!!", thread->GetName().c_str());
            thread->run();
        }
        i++;
    }
    return true;
}

bool PipelineThreadManager::stop() {
    std::map<std::string, std::shared_ptr<PipelineThread>>::iterator i = thread_map_.begin();
    while (i != thread_map_.end()) {
        i->second->requestExit();
        i++;
    }
    thread_map_.clear();
    return true;
}

bool PipelineThreadManager::stopwait() {
    std::map<std::string, std::shared_ptr<PipelineThread>>::iterator i = thread_map_.begin();
    while (i != thread_map_.end()) {
        if (0 != (i->first.compare(PIPELINE_THREAD_MAIN)))
            i->second->requestExitAndWait();
        else
            i->second->requestExit();
        i++;
    }
    thread_map_.clear();
    return true;
}

PipelineThread::PipelineThread(const std::string &name) :  name_(name), recurse_(0), report_count_(0) {
    msg_wait_timeout_ = 0;
    msg_timeout_action_ = NULL;

    is_main_ = (0 == name.compare(PIPELINE_THREAD_MAIN));
    if (is_main_) {
        // mThread = getThreadId();
    }
}

int PipelineThread::run(const char *name, int32_t priority, size_t stack) {

    if (!is_main_)
        return ThreadWrap::run(name_.c_str(), priority, stack);

    {

        std::lock_guard<std::mutex> lock(mutex_);
        if (thread_->get_id() != std::this_thread::get_id()) {
            LOGD(
                "Thread (this=%p): don't call run() from this "
                "Thread object's thread. It's a guaranteed deadlock!",
                this);

            return -1;
        }

        /// reset status and exitPending to their default value, so we can
        /// try again after an error happened (either below, or in readyToRun())
        // mStatus = NO_ERROR;
        mExitPending = false;

        /// hold a strong reference on ourself
        mHoldSelf = ThreadWrap::shared_from_this();

        mRunning = true;
    }

    _threadLoop(this);
    return 0;
}

bool PipelineThread::OnPipelineMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink) {
    // sp<PipelineNode> node = sink->GetNode().promote();
    // LOGD("Thread(%s)'s OnPipelineMsg(%s->%s):%p,%d",name_.c_str(),
    //                                msg->GetName(),node->GetName().c_str(),msg.get(),msg->getStrongCount());
    // ShareDataFactoryManager::getInstance().ShowUsage();

    int queueSize;
    MsgItem msgitem(msg, sink);
    msg_mutex_.lock();
    msg_queue_.push_back(msgitem);
    queueSize = msg_queue_.size();
    msg_mutex_.unlock();
    // msg_condition_.signal();
    msg_condition_.notify_one();

    static int lastQueueSize = 0;
    if (lastQueueSize < queueSize) {
        LOGD("Thread(%s) MessgeQueue size:%d,last:%d", name_.c_str(), queueSize, lastQueueSize);
        lastQueueSize = queueSize;
    }
    return true;
}

bool PipelineThread::SetMsgTimeout(uint64_t timeout, PipelineThread_Timeout action, void *arg) {
    msg_wait_timeout_ = timeout;
    msg_timeout_action_ = action;
    msg_timeout_action_arg_ = arg;
    return true;
}

int PipelineThread::readyToRun() {
#if 0
    std::string threadname = "per:";
    threadname += name_;
    foundationSetThreadName(threadname.c_str());

    return Thread::readyToRun();
#endif
    // TODO: 设置线程名
    return 0;
}

bool PipelineThread::threadLoop() {
    recurse_++;

    MsgItem msgnull;
    std::list<MsgItem> msgqueue;

    {
        std::unique_lock<std::mutex> lock(msg_mutex_);

        while (msg_queue_.empty()) {
            if (recurse_ > 1) {
                recurse_--;
                return true;
            }
            if (msg_wait_timeout_) {
                auto status = msg_condition_.wait_for(lock, std::chrono::milliseconds(msg_wait_timeout_));
                if (status == std::cv_status::timeout) {
                    if (msg_timeout_action_) {
                        msg_timeout_action_(msg_timeout_action_arg_);
                    }
                }
            } else
                msg_condition_.wait(lock);
        }
        msgqueue.swap(msg_queue_);

    }

    // LOGD("Thread %s process message(%d)!!!!!",
    // name_.c_str(),msgqueue.size());

    std::list<MsgItem>::iterator i = msgqueue.begin();

    std::map<PipelineNode *, std::vector<PipelineNodeMsgItem>> messageMap;

    for (; i != msgqueue.end(); i++) {
        if (exitPending())
            break;

        MsgItem &msgitem = *i;
        PipelineNode *node = msgitem.sink_->GetNode().lock().get();
        std::map<PipelineNode *, std::vector<PipelineNodeMsgItem>>::iterator iter = messageMap.find(node);
        if (iter == messageMap.end()) {
            messageMap[node] = std::vector<PipelineNodeMsgItem>();
            iter = messageMap.find(node);
            assert(iter != messageMap.end());
        }
        PipelineNodeMsgItem msg ;
        msg.msg = &(msgitem.msg_);
        msg.sink = &(msgitem.sink_);
        iter->second.push_back(msg);
    }

    std::map<PipelineNode *, std::vector<PipelineNodeMsgItem>>::iterator iter = messageMap.begin();
    for (; iter != messageMap.end(); iter++) {
        if (exitPending())
            break;

        PipelineNode *node = iter->first;
        std::vector<PipelineNodeMsgItem> &msgs = iter->second;
        node->ProcessMsgs(msgs);
    }
    messageMap.clear();
    msgqueue.clear();
    recurse_--;

    return !exitPending();
}

bool PipelineThread::ProcessMessages() {
    if (thread_->get_id() != std::this_thread::get_id()) {
        /// called from other thread, do nothing
        return false;
    }
    if (recurse_ > 6) {
        assert(0);
        return false;
    }

    return threadLoop();
}
