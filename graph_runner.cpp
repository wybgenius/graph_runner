#include <cstdlib>

#include "error_code.h"
#include "graph_runner.h"

namespace graphrunner
{

GraphRunner::GraphRunner(std::unique_ptr<Graph>&& pGraph,
                         int nThreads,
                         int maxTaskNum) :
    mpGraph(std::move(pGraph)), mpThreadPool(new ThreadPool(nThreads, maxTaskNum))
{
}

GraphRunner::~GraphRunner()
{
    Stop();
}

int GraphRunner::Start()
{
    return SUCC;
}

int GraphRunner::Stop()
{
    return SUCC;
}

int GraphRunner::Wait()
{
    //TODO: WaitableNumeric Wait
    //TODO: 返回全局的error code

    return SUCC;
}

class GraphRunner::ExecuteTask : public ThreadPool::Task
{
public:
    ExecuteTask(GraphRunner& runner, std::shared_ptr<Context>& pContext, const std::string& opName) :
        mRunner(runner), mpContext(pContext), mOpName(opName)
    { }
    virtual ~ExecuteTask() { }
    virtual void Submitted() { }
    virtual int Wait(int &rtn) { return SUCC; }
    virtual void Wait() { }
    virtual void Run()
    {
        mRunner.RunOpInternal(mpContext, mOpName);
    }

private:
    GraphRunner& mRunner;
    std::shared_ptr<Context> mpContext;
    std::string mOpName;
};

int GraphRunner::SubmitInternal(const std::vector<std::string>& initNameList,
                                std::vector<std::unique_ptr<IOpData>>& initDataList)
{
    int rtn;

    std::shared_ptr<Context> pContext(new Context());

    int opSize = mpGraph->OpSize();
    for (int i = 0; i < opSize; i++)
    {
        int opInputDataSize = mpGraph->GetOpInputDataSize(i);
        std::string opName = mpGraph->GetOpName(i);
        rtn = pContext->InitOpInputDataContext(opName, opInputDataSize);
        CHECK_RTN(rtn);
    }

    //TODO: WaitableNumeric Add opSize

    //init fake op run done
    RunOpDone(pContext, initNameList, initDataList);

    return SUCC;
}

class GraphRunner::OpTaskVisitor : public IOpTaskVisitor
{
public:
    OpTaskVisitor(GraphRunner& runner, std::shared_ptr<Context>& pContext) :
        mRunner(runner), mpContext(pContext)
    { }
    virtual ~OpTaskVisitor() { }

    virtual void Visit(SyncOpTaskBase& opTask) override
    {
        mRunner.RunSyncOp(mpContext, opTask);
    }

    //virtual void Visit(ASyncOpTaskBase& opTask) override

private:
    GraphRunner& mRunner;
    std::shared_ptr<Context> mpContext;
};

void GraphRunner::RunOpInternal(std::shared_ptr<Context>& pContext, const std::string& opName)
{
    int rtn;

    std::shared_ptr<IOpTask> pTask;
    rtn = mpGraph->GetOp(opName, pTask);
    if (rtn != SUCC)
    {
        abort();
    }

    OpTaskVisitor taskVisitor(*this, pContext);
    pTask->Accept(taskVisitor);
}

void GraphRunner::RunSyncOp(std::shared_ptr<Context>& pContext, SyncOpTaskBase& opTask)
{
    int rtn;

    int inputNameListSize = opTask.mInputNameList.size();
    int outputNameListSize = opTask.mOutputNameList.size();

    std::unique_ptr<OpInputDataList> pInputList(new OpInputDataList(inputNameListSize));
    for (int i = 0; i < inputNameListSize; i++)
    {
        const void* data = NULL;
        rtn = pContext->GetOpOutputData(opTask.mInputNameList[i], data);
        if (rtn != SUCC)
        {
            abort();
        }
        pInputList->SetData(i, data);
    }

    std::vector<std::unique_ptr<IOpData>> outputList;
    rtn = opTask.Run(std::move(pInputList), outputList);

    int outputListSize = outputList.size();
    if (rtn == SUCC && outputNameListSize != outputListSize)
    {
        rtn = ERR;
    }
    if (rtn != SUCC)
    {
        //TODO: 设置全局的error info（atomic<T>），wait的时候返回error code

        //TODO: task run可能会出现类型不对、参数数量不对等问题，需要考虑更好的异常处理
        //TODO: 目前考虑将output data设置为null，后续op都跳过，wait返回错误
        outputList.resize(outputNameListSize);
        for (int i = 0; i < outputNameListSize; i++)
        {
            outputList[i].reset(new NullOpData());
        }
    }

    RunOpDone(pContext, opTask.mOutputNameList, outputList);
}

void GraphRunner::RunOpDone(std::shared_ptr<Context>& pContext,
             const std::vector<std::string>& outputNameList,
             std::vector<std::unique_ptr<IOpData>>& outputList)
{
    int rtn;

    int outputNameListSize = outputNameList.size();
    for (int i = 0; i < outputNameListSize; i++)
    {
        const std::string& outputName = outputNameList[i];

        rtn = pContext->SetOpOutputData(outputName, std::move(outputList[i]));
        if (rtn != SUCC)
        {
            abort();
        }

        std::vector<std::string> nextOpNameList;
        rtn = mpGraph->GetOpNameListByInputName(outputName, nextOpNameList);
        if (rtn != SUCC)
        {
            abort();
        }

        int nextOpNameListSize = nextOpNameList.size();
        for (int j = 0; j < nextOpNameListSize; j++)
        {
            const std::string& nextOpName = nextOpNameList[j];
            if (pContext->IncrAndCheckOpInputDataContext(nextOpName))
            {
                //TODO: 这里需要细考虑，直接加到thread pool应该会有死锁问题，buffer设置小的情况下，都会阻塞住，然后又消费不掉
                rtn = mpThreadPool->Submit(std::make_shared<ExecuteTask>(*this, pContext, nextOpName));
                if (rtn != SUCC)
                {
                    abort();
                }
            }
        }
    }

    //TODO: WaitableNumeric - 1
}

}
