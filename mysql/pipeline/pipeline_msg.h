#ifndef PIPELINE2_PIPELINE_MSG_H
#define PIPELINE2_PIPELINE_MSG_H

#include <string>
#include <memory>
#include <vector>
#include <mutex>

#ifndef PIPELINE_FOURCC
#define PIPELINE_FOURCC(a, b, c, d) \
((((uint32_t)a) << 0) | (((uint32_t)b) << 8) | (((uint32_t)c) << 16) | (((uint32_t)d) << 24))
#endif

#define PIPELINE_SYSMSG PIPELINE_FOURCC('s', 'y', 's', '\0')

/**
 * PipelineMsg:
 *      #define PIPELINE_CAMMSG PIPELINE_FOURCC('c', 'a', 'm', '\0')
 *
 *      class CameraMsg : public PipelineMsg {
 *      public:
 *          CameraMsg() {
 *              fourcc_ = PIPELINE_CAMMSG;
 *          }
 *          virtual ~CameraMsg() {}
 *          ...
 *          virtual void Reset() override {
 *              PipelineMsg::Reset();
 *          }
 *      };
*/

class PipelineMsg {
public:
    PipelineMsg() {
        fourcc_ = PIPELINE_SYSMSG;
    }
    virtual ~PipelineMsg() {}

    virtual uint32_t GetFourcc() {
        return fourcc_;
    }

    // !!!重置Msg状态
    virtual void Reset() {
    }

protected:
    uint32_t fourcc_;
};

using MsgPtr = std::shared_ptr<PipelineMsg>;

/**
 * PipelineMsgPool:
 *      std::shared_ptr<CameraMsg> cam_msg = PipelineMsgPool<CameraMsg>::GetInstance().AcquireData();
 */

template<typename PipelineMsgT>
class PipelineMsgPool {
public:
    PipelineMsgPool(const PipelineMsgPool&) = delete;
    PipelineMsgPool& operator=(const PipelineMsgPool&) = delete;

    static PipelineMsgPool<PipelineMsgT>& GetInstance() {
        static PipelineMsgPool<PipelineMsgT> instance;
        return instance;
    }

    std::shared_ptr<PipelineMsgT> AcquireData() {
        std::lock_guard<std::mutex> lock(mutex_);

        std::shared_ptr<PipelineMsgT> data = nullptr;
        if(!free_array_.empty()) {
            data = free_array_.back();
            free_array_.pop_back();
        } else {
            allocated_++;

            data = std::make_shared<PipelineMsgT>();
        }

        // 创建带自定义删除器的控制块
        auto recycler = std::shared_ptr<void>(
            nullptr,
            [this, data](void*) {
                this->ReleaseData(data);
            }
            );
        return std::shared_ptr<PipelineMsgT>(recycler, data.get());
    }

    void ReleaseData(std::shared_ptr<PipelineMsgT> data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data->Reset(); // 清除之前的状态
        free_array_.push_back(data);
    }

    int GetAllocated() const {
        return allocated_;
    }
    int GetFree() const {
        return free_array_.size();
    }

private:
    PipelineMsgPool() = default;
    ~PipelineMsgPool() = default;

private:
    std::mutex mutex_;

    int allocated_ {0};
    std::vector<std::shared_ptr<PipelineMsgT>> free_array_;
};

#endif // PIPELINE2_PIPELINE_MSG_H
