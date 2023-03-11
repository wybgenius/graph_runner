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
        mRunner.RunInternal(mpContext, mOpName);
    }

private:
    GraphRunner& mRunner;
    std::shared_ptr<Context> mpContext;
    std::string mOpName;
};

int GraphRunner::SubmitInternal(const std::vector<std::string>& initNameList,
                                std::vector<std::unique_ptr<IOpData>>& initDataList)
{
    //TODO: ret处理
    //int ret;

    std::shared_ptr<Context> pContext(new Context());

    int opSize = mpGraph->OpSize();
    for (int i = 0; i < opSize; i++)
    {
        int opInputDataSize = mpGraph->GetOpInputDataSize(i);
        std::string opName = mpGraph->GetOpName(i);
        pContext->InitOpInputDataContext(opName, opInputDataSize);
    }

    //TODO: WaitableNumeric Add opSize

    //init fake op run done
    RunDone(pContext, initNameList, initDataList);

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

void GraphRunner::RunInternal(std::shared_ptr<Context>& pContext, const std::string& opName)
{
    //int ret;

    std::shared_ptr<IOpTask> pTask;
    mpGraph->GetOp(opName, pTask);

    OpTaskVisitor taskVisitor(*this, pContext);
    pTask->Accept(taskVisitor);
}

void GraphRunner::RunSyncOp(std::shared_ptr<Context>& pContext, SyncOpTaskBase& opTask)
{
    //TODO: ret处理
    //TODO: 并发问题考虑
    //int ret;

    int inputNameListSize = opTask.mInputNameList.size();
    int outputNameListSize = opTask.mOutputNameList.size();

    std::unique_ptr<OpInputDataList> pInputList(new OpInputDataList(inputNameListSize));
    for (int i = 0; i < inputNameListSize; i++)
    {
        const void* data = pContext->GetOpOutputData(opTask.mInputNameList[i]);
        pInputList->SetData(i, data);
    }

    std::vector<std::unique_ptr<IOpData>> outputList(outputNameListSize);
    opTask.Run(std::move(pInputList), outputList);
    //check ret
    //check outputList.size() == outputNameListSize

    RunDone(pContext, opTask.mOutputNameList, outputList);
}

void GraphRunner::RunDone(std::shared_ptr<Context>& pContext,
             const std::vector<std::string>& outputNameList,
             std::vector<std::unique_ptr<IOpData>>& outputList)
{
    //TODO: ret处理
    //int ret;

    int outputNameListSize = outputNameList.size();
    for (int i = 0; i < outputNameListSize; i++)
    {
        const std::string& outputName = outputNameList[i];

        pContext->SetOpOutputData(outputName, std::move(outputList[i]));

        std::vector<std::string> nextOpNameList;
        mpGraph->GetOpNameListByInputName(outputName, nextOpNameList);

        int nextOpNameListSize = nextOpNameList.size();
        for (int j = 0; j < nextOpNameListSize; j++)
        {
            const std::string& nextOpName = nextOpNameList[j];
            if (pContext->IncrAndCheckOpInputDataContext(nextOpName))
            {
                //TODO: 这里需要细考虑，直接加到thread pool应该会有死锁问题，buffer设置小的情况下，都会阻塞住，然后又消费不掉
                mpThreadPool->Submit(std::make_shared<ExecuteTask>(*this, pContext, nextOpName));
            }
        }
    }

    //TODO: WaitableNumeric - 1
}

}
