#include "thread.h"

ThreadWrap::ThreadWrap(bool canCallJava)
    :   mCanCallJava(canCallJava),
    mStatus(0),
    mExitPending(false), mRunning(false)
{
}

ThreadWrap::~ThreadWrap()
{
}

int ThreadWrap::readyToRun()
{
    return 0;
}

int ThreadWrap::run(const char* name, int32_t priority, size_t stack)
{
    std::lock_guard lock(mutex_);

    if (mRunning) {
        return 0;
    }

    // reset status and exitPending to their default value, so we can
    // try again after an error happened (either below, or in readyToRun())
    mStatus = 0;
    mExitPending = false;

    // hold a strong reference on ourself
    mHoldSelf = ThreadWrap::shared_from_this();

    mRunning = true;

    bool res;

    thread_ = std::make_shared<std::thread>(_threadLoop, this);
    if (thread_ == nullptr) {
        mStatus = -1;   // something happened!
        mRunning = false;
        mHoldSelf.reset();  // "this" may have gone away after this.

        return -1;
    }

    // Do not refer to mStatus here: The thread is already running (may, in fact
    // already have exited with a valid mStatus result). The NO_ERROR indication
    // here merely indicates successfully starting the thread and does not
    // imply successful termination/execution.
    return 0;

    // Exiting scope of mutex_ is a memory barrier and allows new thread to run
}

int ThreadWrap::_threadLoop(void* user)
{
    ThreadWrap* const self = static_cast<ThreadWrap*>(user);

    std::shared_ptr<ThreadWrap> strong(self->mHoldSelf);
    std::weak_ptr<ThreadWrap> weak(strong);
    self->mHoldSelf.reset();

    bool first = true;

    do {
        bool result;
        if (first) {
            first = false;
            self->mStatus = self->readyToRun();
            result = (self->mStatus == 0);

            if (result && !self->exitPending()) {
                // Binder threads (and maybe others) rely on threadLoop
                // running at least once after a successful ::readyToRun()
                // (unless, of course, the thread has already been asked to exit
                // at that point).
                // This is because threads are essentially used like this:
                //   (new ThreadSubclass())->run();
                // The caller therefore does not retain a strong reference to
                // the thread and the thread would simply disappear after the
                // successful ::readyToRun() call instead of entering the
                // threadLoop at least once.
                result = self->threadLoop();
            }
        } else {
            result = self->threadLoop();
        }

        // establish a scope for mutex_
        {
            std::lock_guard _l(self->mutex_);
            if (result == false || self->mExitPending) {
                self->mExitPending = true;
                self->mRunning = false;

                // note that interested observers blocked in requestExitAndWait are
                // awoken by broadcast, but blocked on mutex_ until break exits scope
                self->mThreadExitedCondition.notify_all();
                break;
            }
        }

        // Release our strong reference, to let a chance to the thread
        // to die a peaceful death.
        strong.reset();
        // And immediately, re-acquire a strong reference for the next loop
        strong = weak.lock();
    } while(strong != 0);

    return 0;
}

void ThreadWrap::requestExit()
{
    std::lock_guard lock(mutex_);
    mExitPending = true;
}

int ThreadWrap::requestExitAndWait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (thread_->get_id() == std::this_thread::get_id()) {
        printf(
            "Thread (this=%p): don't call waitForExit() from this "
            "Thread object's thread. It's a guaranteed deadlock!",
            this);
        return -1;
    }

    mExitPending = true;

    while (mRunning == true) {
        mThreadExitedCondition.wait(lock);
    }
    // This next line is probably not needed any more, but is being left for
    // historical reference. Note that each interested party will clear flag.
    mExitPending = false;
    return mStatus;
}

int ThreadWrap::join()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (thread_->get_id() == std::this_thread::get_id()) {
        printf(
            "Thread (this=%p): don't call join() from this "
            "Thread object's thread. It's a guaranteed deadlock!",
            this);
        return -1;
    }

    while (mRunning == true) {
        mThreadExitedCondition.wait(lock);
    }

    return mStatus;
}

bool ThreadWrap::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return mRunning;
}

bool ThreadWrap::exitPending() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return mExitPending;
}
