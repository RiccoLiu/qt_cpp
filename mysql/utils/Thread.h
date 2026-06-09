#ifndef THREAD_WRAP_H
#define THREAD_WRAP_H

#include <stdint.h>
#include <memory.h>
#include <mutex>
#include <condition_variable>

#include "utils_global.h"


enum {
    PRIORITY_LOWEST         = 19,//FOUNDATION_PRIORITY_LOWEST,
    PRIORITY_BACKGROUND     = 10,//FOUNDATION_PRIORITY_BACKGROUND,
    PRIORITY_NORMAL         = 0, //FOUNDATION_PRIORITY_NORMAL,
    PRIORITY_FOREGROUND     = -2,//FOUNDATION_PRIORITY_FOREGROUND,
    PRIORITY_DISPLAY        = -4,//FOUNDATION_PRIORITY_DISPLAY,
    PRIORITY_URGENT_DISPLAY = -8,//FOUNDATION_PRIORITY_URGENT_DISPLAY,
    PRIORITY_AUDIO          =-16,//FOUNDATION_PRIORITY_AUDIO,
    PRIORITY_URGENT_AUDIO   =-19,//FOUNDATION_PRIORITY_URGENT_AUDIO,
    PRIORITY_HIGHEST        =-20,//FOUNDATION_PRIORITY_HIGHEST,
    PRIORITY_DEFAULT        = 0,//FOUNDATION_PRIORITY_DEFAULT,
    PRIORITY_MORE_FAVORABLE = -1,//FOUNDATION_PRIORITY_MORE_FAVORABLE,
    PRIORITY_LESS_FAVORABLE = +1,//FOUNDATION_PRIORITY_LESS_FAVORABLE,
};

class ThreadWrap : public std::enable_shared_from_this<ThreadWrap> {
public:
    // Create a Thread object, but doesn't create or start the associated
    // thread. See the run() method.
    ThreadWrap(bool canCallJava = true);
    virtual ~ThreadWrap();

    // Start the thread in threadLoop() which needs to be implemented.
    virtual int run(const char* name = 0, int32_t priority = PRIORITY_DEFAULT, size_t stack = 0);

    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void requestExit();

    // Good place to do one-time initializations
    virtual int readyToRun();

    // Call requestExit() and wait until this object's thread exits.
    // BE VERY CAREFUL of deadlocks. In particular, it would be silly to call
    // this function from this object's thread. Will return WOULD_BLOCK in
    // that case.
    int requestExitAndWait();

    // Wait until this object's thread exits. Returns immediately if not yet running.
    // Do not call from this object's thread; will return WOULD_BLOCK in that case.
    int join();

    // Indicates whether this thread is running or not.
    bool isRunning() const;

protected:
    // exitPending() returns true if requestExit() has been called.
    bool exitPending() const;

protected:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool threadLoop() = 0;

protected:
    ThreadWrap& operator=(const ThreadWrap&);
    static int _threadLoop(void* user);
    const bool mCanCallJava;
    // always hold mLock when reading or writing

    // std::thread::id mThread;
    std::shared_ptr<std::thread> thread_;

    mutable std::mutex mutex_;
    std::condition_variable mThreadExitedCondition;
    int mStatus;
    // note that all accesses of mExitPending and mRunning need to hold mLock
    volatile bool mExitPending;
    volatile bool mRunning;
    std::shared_ptr<ThreadWrap> mHoldSelf;
};

#endif // THREAD_WRAP_H
