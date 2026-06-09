#ifndef THREAD_H
#define THREAD_H

#include <string>
#include <memory>
#include <map>
#include <list>
#include <mutex>
#include <condition_variable>

#include <singleton.h>
#include <Thread.h>
#include "pipeline_msg.h"
#include "sink.h"

#define PIPELINE_THREAD_MAIN "main"

class PipelineNode;

/**
 * PipelineThread: Thread for pipeline.
 */
typedef void (*PipelineThread_Timeout)(void *);

class PipelineThread  : public ThreadWrap {
    friend class PipelineNode;

public:
    PipelineThread(const std::string &name);
    virtual ~PipelineThread() {}
    const std::string &GetName() { return name_; }

    // bool LoadYAML(const YAML::Node &config) { return true; }
    bool OnPipelineMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink);
    bool ProcessMessages();
    bool SetMsgTimeout(uint64_t timeout, PipelineThread_Timeout action, void *arg);

    /// Start the thread in threadLoop() which needs to be implemented.
    virtual int run(const char *name = 0, int32_t priority = PRIORITY_DEFAULT, size_t stack = 0);
    virtual int readyToRun();

private:
    /// Derived class must implement threadLoop(). The thread starts its life
    /// here. There are two ways of using the Thread object:
    /// 1) loop: if threadLoop() returns true, it will be called again if
    ///          requestExit() wasn't called.
    /// 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool threadLoop();

private:
    std::string name_;
    bool is_main_;
    int recurse_;

    class MsgItem {
    public:
        MsgItem() {}
        MsgItem(std::shared_ptr<PipelineMsg> &m, std::shared_ptr<PipelineSink> &s) : msg_(m), sink_(s) {}
        std::shared_ptr<PipelineMsg> msg_;
        std::shared_ptr<PipelineSink> sink_;
    };

    std::mutex msg_mutex_;
    std::condition_variable msg_condition_;
    std::list<MsgItem> msg_queue_;
    int report_count_;

    uint64_t msg_wait_timeout_;
    void *msg_timeout_action_arg_;
    PipelineThread_Timeout msg_timeout_action_;

    DISALLOW_COPY_AND_ASSIGN(PipelineThread);
};

/**
 * PipelineThreadManager: Thread Manager
 */
class PipelineThreadManager : public Singleton<PipelineThreadManager> {
public:
    std::shared_ptr<PipelineThread> GetThread(const std::string &name);

    bool Start();
    bool Stop(bool wait = false) {
        if (!wait)
            return stop();
        else
            return stopwait();
    }

private:
    bool stop();
    bool stopwait();

private:
    std::map<std::string, std::shared_ptr<PipelineThread>> thread_map_;
};


#endif // THREAD_H
