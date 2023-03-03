#include <future>

#include "thread_pool.h"
#include "error_code.h"

namespace graphrunner
{

ThreadPool::Task::Task(const Function &function)
{
    mpTask.reset(new std::packaged_task<int()>(function));
    mFuture = mpTask->get_future();
}

ThreadPool::Task::Task()
{
}

ThreadPool::Task::~Task()
{
}

void ThreadPool::Task::Submitted()
{
}

void ThreadPool::Task::Run()
{
    (*mpTask.get())();
}

int ThreadPool::Task::Wait(int &rtn)
{
    //TODO
    return SUCC;
}

void ThreadPool::Task::Wait()
{
    auto& future = GetFuture();
    if (future.valid())
        future.wait();
}

std::future<int> &ThreadPool::Task::GetFuture()
{
    return mFuture;
}

ThreadPool::ThreadPool(int nThreads, int maxTaskNum)
    :mNThreads(nThreads), mMaxTaskNum(maxTaskNum)
{
    //TODO
}

ThreadPool::~ThreadPool()
{
    //TODO
}

}
