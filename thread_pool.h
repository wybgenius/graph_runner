#ifndef GRAPHRUNNER_THREAD_POOL_H_
#define GRAPHRUNNER_THREAD_POOL_H_

#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <future>

#include "object_buffer.h"
#include "error_code.h"

namespace graphrunner
{

class ThreadPool
{
public:
    ThreadPool(int nThreads, int maxTaskNum);

    virtual int GetThreadsNum() { return mNThreads; }
    virtual int GetMaxTaskNum() { return mMaxTaskNum; }

    virtual ~ThreadPool();

public:
    typedef std::function<int()> Function;

    class Task
    {
    public:
        Task(const Function &function);

        Task();

        virtual ~Task();

        virtual void Submitted();

        virtual void Run();

        virtual int Wait(int &rtn);

        virtual void Wait();

    protected:
        virtual std::future<int> &GetFuture();

    private:
        std::future<int> mFuture;
        std::shared_ptr<std::packaged_task<int()>> mpTask;
    };

    typedef std::shared_ptr<Task> TaskPtr;

public:
    virtual int Submit(TaskPtr pTask) { return SUCC; }

    virtual int TrySubmit(TaskPtr pTask) { return SUCC; }

    virtual int GetRunningTaskSize() const { return SUCC; }

private:
    ObjectBuffer<TaskPtr> *mpTaskBuffer;
    std::vector<std::thread*> mThreads;

    int mNThreads;
    int mMaxTaskNum;
};

}

#endif
