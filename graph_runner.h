#ifndef GRAPHRUNNER_GRAPHRUNNER_H_
#define GRAPHRUNNER_GRAPHRUNNER_H_

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "error_code.h"
#include "constants.h"
#include "thread_pool.h"
#include "graph.h"
#include "context.h"

namespace graphrunner
{

class GraphRunner
{
public:
    GraphRunner(std::unique_ptr<Graph>&& pGraph,
                int nThreads,
                int maxTaskNum);

    int Start();
    int Stop();

    template <typename Type>
    int Submit(std::unique_ptr<Type>&& pInput);

    int Wait();

    virtual ~GraphRunner();

private:
    void RunInternal(std::shared_ptr<Context>& context, std::string& opName);

private:
    class Task : public ThreadPool::Task
    {
    public:
        Task(GraphRunner& runner, std::shared_ptr<Context>& pContext, std::string opName) : 
            mRunner(runner), mpContext(pContext), mOpName(opName)
        { }
        virtual ~Task() { }
        virtual void Submitted() { }
        virtual int Wait(int &rtn) { return SUCC; }
        virtual void Wait() { }
        virtual void Run()
        {
            mRunner.RunInternal(mpContext, mOpName);
        }

        GraphRunner& mRunner;
        std::shared_ptr<Context> mpContext;
        std::string mOpName;
    };

private:
    std::unique_ptr<Graph> mpGraph;

    std::unique_ptr<ThreadPool> mpThreadPool;
};


template <typename Type>
int GraphRunner::Submit(std::unique_ptr<Type>&& pInput)
{
    //TODO: ret处理
    int ret = SUCC;

    std::shared_ptr<Context> pContext(new Context());

    int opSize = mpGraph->OpSize();
    for (int i = 0; i < opSize; i++)
    {
        int opInputDataSize = mpGraph->GetOpInputDataSize(i);
        std::string opName = mpGraph->GetOpName(i);
        pContext->InitOpInputDataContext(opName, opInputDataSize);
    }

    //TODO: WaitableNumeric Add opSize

    std::unique_ptr<OpOutput<Type>> opOutput(new OpOutput<Type>);
    opOutput->SetNewData(std::move(pInput));
    pContext->SetOpOutputData(
        Constants::sGraphRunnerInitInputName,
        std::move(opOutput));

    std::vector<std::string> headOpNameList;
    mpGraph->GetOpNameListByInputName(
        Constants::sGraphRunnerInitInputName,
        headOpNameList);
    int headOpNameListSize = headOpNameList.size();
    for (int i = 0; i < headOpNameListSize; i++)
    {
        std::string opName = headOpNameList[i];
        //TODO: use IncrAndCheck?
        pContext->IncrOpInputDataContext(opName);
        mpThreadPool->Submit(std::make_shared<Task>(*this, pContext, opName));
    }

    return ret;
}

}

#endif
