#ifndef MSG_H
#define MSG_H

// #include <unistd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#include <mutex>
#include <compiler.h>
#include <sharedt.h>

class PipelineSource;
class PipelineMsgQueueBase;

struct PipelineTimeStamp {
    uint64_t sec;    ///< 秒
    uint64_t nsec;   ///< 纳秒
    uint64_t _time;  ///< 用于记录自定义时间信息
};

inline bool operator<(const PipelineTimeStamp &v1, const PipelineTimeStamp &v2) {
    return (v1.sec < v2.sec) ||
            ((v1.sec == v2.sec) && ((v1.nsec < v2.nsec) ||
             ((v1.nsec == v2.nsec) && (v1._time < v2._time))));
}

inline bool operator>(const PipelineTimeStamp &v1, const PipelineTimeStamp &v2) {
    return (v1.sec > v2.sec) || ((v1.sec == v2.sec) && ((v1.nsec > v2.nsec) || ((v1.nsec == v2.nsec) && (v1._time >
    v2._time))));
}

/**
 * PipelineMsg is data instance that pass through the pipeline.
 */
class PipelineMsg : public  std::enable_shared_from_this<PipelineMsg> {
public:
    PipelineMsg(std::weak_ptr<PipelineSource> &source)
        : source_(source), fourcc_(0), time_stamp_{} {}
    virtual ~PipelineMsg() {}

    bool GetTimestamp(PipelineTimeStamp &t) {
        t = time_stamp_;
        return (0UL != t.sec) || (0UL != t.nsec);
    }
    uint64_t GetTimestamp() {
        return time_stamp_.sec * 1000000000UL + time_stamp_.nsec;
    }
    uint32_t GetFourCC() {
        return fourcc_;
    }
    void GetFourCC(char *foucc);

    std::shared_ptr<PipelineMsg> GetMessage() {
        return shared_from_this();
    }
    const char *GetName();

    void SetQueue(std::shared_ptr<PipelineMsgQueueBase> queue) {
        queue_ = queue;
    }
    void SetTimestamp(PipelineTimeStamp &t) {
        time_stamp_ = t;
    }
    void SetTimestamp(uint64_t t) {
        time_stamp_.sec = t / 1000000000UL;
        time_stamp_.nsec = t % 1000000000UL;
    }

    /// lock this object for write after the msg is sent out
    void LockWrite() { mutex_.lock(); }
    void UnlockWrite() { mutex_.unlock(); }

protected:
    std::weak_ptr<PipelineSource> source_;
    std::shared_ptr<PipelineMsgQueueBase> queue_;

    uint32_t fourcc_;
    PipelineTimeStamp time_stamp_;

    std::mutex mutex_;

private:
    DISALLOW_COPY_AND_ASSIGN(PipelineMsg);
};

/**
 * PipelineMsgQueueBase
 */
class PipelineMsgQueueBase : virtual public ShareDataFactoryI {
public:
    PipelineMsgQueueBase(std::weak_ptr<PipelineSource> &source, const char *name)
        : ShareDataFactoryI(name), source_(source)  {}
    virtual ~PipelineMsgQueueBase() {}

    template <typename PipelineMsgQueueT>
    PipelineMsgQueueT *UCast2() {
        return static_cast<PipelineMsgQueueT *>(this);
    }

protected:
    std::weak_ptr<PipelineSource> source_;

private:
    DISALLOW_COPY_AND_ASSIGN(PipelineMsgQueueBase);
};

/**
 * PipelineMsgQueue Template Helper
 */
template <typename PipelineMsgT>
class PipelineMsgQueue : public PipelineMsgQueueBase, public std::enable_shared_from_this<PipelineMsgQueue<PipelineMsgT>> {
public:
    PipelineMsgQueue(std::weak_ptr<PipelineSource> &source)
        : ShareDataFactoryI(PipelineMsgT::ClassName()), PipelineMsgQueueBase(source, PipelineMsgT::ClassName()) {
        registed_ = true;
        ShareDataFactoryManager::GetInstance().AddWeekFactory(this);
    }
    virtual ~PipelineMsgQueue() { Uninit(); }

    std::shared_ptr<PipelineMsgT> AcquireMessageT() {
        std::shared_ptr<PipelineMsgT> ret = nullptr;
        mutex_.lock();
        if (!free_array_.empty()) {
            std::weak_ptr<PipelineMsgT> p = free_array_.back();
            free_array_.pop_back();
            ret = p.lock();
        }
        if (ret == nullptr) {
            allocated_++;
        }
        mutex_.unlock();
        if (ret == nullptr) {
            ret = std::make_shared<PipelineMsgT>(source_);
            ret->SetQueue(shared_from_this());
        }
        return ret;
    }

    void ReleaseMessageT(PipelineMsgT *data) {
        mutex_.lock();
        free_array_.push_back(data);
        mutex_.unlock();
    }

    virtual void Uninit() {
        mutex_.lock();
        while (!free_array_.empty()) {
            free_array_.pop_back();
        }
        allocated_ = 0;
        mutex_.unlock();
    }

    virtual int GetFree() {
        return free_array_.size();
    }

private:
    std::vector<std::weak_ptr<PipelineMsgT>> free_array_;

private:
    DISALLOW_COPY_AND_ASSIGN(PipelineMsgQueue);
};


#endif // MSG_H
