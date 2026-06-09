#ifndef UTILS_SHAREDT_H
#define UTILS_SHAREDT_H

#include <mutex>
#include <vector>
#include <singleton.h>

#include "utils_global.h"


class ShareDataFactoryI {
public:
    ShareDataFactoryI(const char* name)
        : name_(name)
        , allocated_(0)
        , registed_(false)
    {
    }
    virtual ~ShareDataFactoryI(){}

    virtual int GetFree() = 0;
    virtual void Uninit() = 0;

    virtual int GetAllocated() {
        return allocated_;
    }
    virtual const char* GetName(){
        return name_;
    }

protected:
    const char* name_;
    int allocated_;

    bool registed_;
    std::mutex mutex_;
};

class ShareDataFactoryManager : public Singleton<ShareDataFactoryManager> {
public:
    void AddFactory(ShareDataFactoryI* factory)
    {
        mutex_.lock();
        wfactory_array_.push_back(factory);
        mutex_.unlock();
    }

    void AddWeekFactory(ShareDataFactoryI* factory)
    {
        mutex_.lock();
        wfactory_array_.push_back(factory);
        mutex_.unlock();
    }

    void Uninit() {
        mutex_.lock();
        while(!factory_array_.empty())
        {
            factory_array_.back()->Uninit();
            factory_array_.pop_back();
        }
        mutex_.unlock();
    }

    void ShowUsage()
    {
        printf("ShareDataFactoryManager::ShowUsage---------------------");
        for(unsigned int i = 0; i < factory_array_.size(); i++) {
            printf("S:%d,\tFree:%d, \tAllocated:%d,\tName:%s",i,
                 factory_array_[i]->GetFree(),factory_array_[i]->GetAllocated(),
                 factory_array_[i]->GetName() );
        }
        for(unsigned int i = 0; i < wfactory_array_.size(); i++) {
            printf("W:%d,\tFree:%d, \tAllocated:%d,\tName:%s",i,
                 wfactory_array_[i]->GetFree(),wfactory_array_[i]->GetAllocated(),
                 wfactory_array_[i]->GetName() );
        }
        printf("-------------------------------------------------------");
    }

private:
    std::mutex mutex_;
    std::vector<ShareDataFactoryI*> factory_array_;
    std::vector<ShareDataFactoryI*> wfactory_array_;
};


#endif // SHAREDT_H
