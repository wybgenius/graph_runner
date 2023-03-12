#ifndef GRAPHRUNNER_THREAD_POOL_H_
#define GRAPHRUNNER_THREAD_POOL_H_

#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <future>

#include "error_code.h"

namespace graphrunner
{

class ThreadPool
{
public:
    ThreadPool(int nThreads, int maxTaskNum) { }

    virtual ~ThreadPool() { }

public:
    typedef std::function<int()> Function;

    class Task
    {
    public:
        Task(const Function &function) { }

        Task() { }

        virtual ~Task() { }

        virtual void Submitted() { }

        virtual void Run() { }

        virtual int Wait(int &rtn) { return SUCC; }

        virtual void Wait() { }

    protected:
        virtual std::future<int> &GetFuture() { return mFuture; }

    private:
        std::future<int> mFuture;
        std::shared_ptr<std::packaged_task<int()>> mpTask;
    };

    typedef std::shared_ptr<Task> TaskPtr;

public:
    virtual int Submit(TaskPtr pTask) { return SUCC; }

private:
    //ObjectBuffer<TaskPtr> *mpTaskBuffer;
    std::vector<std::thread*> mThreads;

    int mNThreads;
    int mMaxTaskNum;
};

}

#endif
