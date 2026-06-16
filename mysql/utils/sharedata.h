#ifndef PIPELINE2_SHAREDATA_H
#define PIPELINE2_SHAREDATA_H

#include <logger2.h>

#include <stdint.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

/**
 * ShareData:
        std::shared_ptr<ShareData> data = std::make_shared<MyShareData>();
        // 消费者线程
        void consumer() {
            data->WaitAvailable(0x1); // 阻塞直到数据就绪
            // 使用 data...
        }

        // 生产者线程
        void producer() {
            uint32_t need = data->Prepare(0x1); // (1) 原子地声明“我要准备 bit0”
            if (need & 0x1) {                   // (2) 只有第一个调用者返回 true
                LoadData();                     // (3) 安全执行，多生产者只会执行一次
                data->SetAvailable(0x1);        // (4) 发布结果
            }
        }
 */

class ShareData {
public:
    ShareData() :  prepare_(0), available_(0) {}
    virtual ~ShareData() {}

    ShareData(const ShareData&) = delete;
    ShareData& operator=(const ShareData&) = delete;

    // 共享数据需要覆写 Reset 函数，保证内存池的数据被清洗
    virtual void Reset() {
        prepare_ = 0;
        available_ = 0;
    }

    bool IsAvailable(uint32_t mask = 0x1) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return  (available_ & mask) == mask;
    }

    void WaitAvailable(uint32_t mask = 0x1) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this, mask] {
            return (available_ & mask) == mask;
        });
    }

    void SetAvailable(uint32_t mask = 0x1) {
        bool do_notify = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if ((available_ & mask) != mask) {
                available_ |= mask;
                do_notify = true;
            }
        }
        if (do_notify) {
            condition_.notify_all(); // 在锁外通知
        }
    }

    // 返回 prepare_ 在 mask 中未准备的好的 bit位， 同时设置 prepare_ 的 mask 位置
    uint32_t Prepare(uint32_t mask = 0x1) {
        std::lock_guard<std::mutex> lock(mutex_);
        uint32_t ret = 0; // need to wait
        uint32_t preparing = prepare_ & mask;
        if(preparing != mask) {
            ret = (~preparing) & mask;
            prepare_ |= mask;
        }
        return ret;
    }

protected:
    mutable std::mutex mutex_;
    std::condition_variable condition_;

    uint32_t prepare_;
    uint32_t available_;
};

/**
 * ShareDataPool(内存池)
 *      class ImageFrame : public ShareData {
 *      ....
 *      };
 *
 *      std::shared_ptr<ImageFrame> image_frame = ShareDataPool<ImageFrame>::GetInstance().AcquireData();
 *      ....
 *
 */
template<typename ShareDataT>
class ShareDataPool {
public:
    ShareDataPool(const  ShareDataPool&) = delete;
    ShareDataPool& operator=(const  ShareDataPool&) = delete;

    static ShareDataPool<ShareDataT> &GetInstance() {
        static ShareDataPool<ShareDataT> theInstance;
        return theInstance;
    }

    std::shared_ptr<ShareDataT> AcquireData() {
        std::lock_guard<std::mutex> lock(mutex_);

        std::shared_ptr<ShareDataT> data = nullptr;
        if(!free_array_.empty()) {
            data = free_array_.back();
            free_array_.pop_back();
        } else {
            allocated_++;

            data = std::make_shared<ShareDataT>();
        }

        // 创建带自定义删除器的控制块
        auto recycler = std::shared_ptr<void>(
            nullptr,
            [this, data](void*) {
                this->ReleaseData(data);
            }
        );
        return std::shared_ptr<ShareDataT>(recycler, data.get());
    }

    void ReleaseData(std::shared_ptr<ShareDataT> data) {
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
    ShareDataPool() = default;
    ~ShareDataPool() = default;

    std::mutex mutex_;

    int allocated_ {0};
    std::vector<std::shared_ptr<ShareDataT>> free_array_;
};

/**
 * ShareDataManager
 */
class ShareDataManager {
public:
    ShareDataManager() {}
    virtual ~ShareDataManager() {}

    bool HasObject(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        bool is_find = false;
        if (auto it = share_data_.find(key); it != share_data_.end()) {
            is_find = true;
        }
        return is_find;
    }

    void AddObject(const std::string& key, std::shared_ptr<ShareData> obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        share_data_[key] = obj;

        condition_.notify_all();
    }

    std::shared_ptr<ShareData> GetObject(const std::string& key, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        std::unique_lock<std::mutex> lock(mutex_);

        auto it = share_data_.find(key);
        if (it != share_data_.end()) {
            return it->second;
        }

        std::shared_ptr<ShareData> obj = nullptr;
        if (timeout > std::chrono::milliseconds(0)) {
            if (std::cv_status::timeout != condition_.wait_for(lock, timeout)) {
                auto find = share_data_.find(key);
                if (find != share_data_.end()) {
                    obj = find->second;
                }
            } else {
                LOGW("GetObject: wait obj '%s' timeout", key.c_str());
            }
        }
        return obj;
    }

    void ReleaseObject(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (auto it = share_data_.find(key); it != share_data_.end()) {
            share_data_.erase(it);
        }
    }

    void ClearObject() {
        std::lock_guard<std::mutex> lock(mutex_);
        share_data_.clear();
        condition_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::unordered_map<std::string, std::shared_ptr<ShareData>> share_data_;
};

#endif // PIPELINE2_SHAREDATA_H
