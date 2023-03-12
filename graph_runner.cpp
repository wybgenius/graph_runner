#include <cstdlib>

#include "error_code.h"
#include "graph_runner.h"

namespace graphrunner
{

GraphRunner::GraphRunner(int nThreads) :
    mNThreads(nThreads)
{
}

GraphRunner::~GraphRunner()
{
    Stop();
}

int GraphRunner::Init(std::unique_ptr<Graph>&& pGraph)
{
    //TODO: 基于graph构建runner内部的数据结构

    mpGraph = std::move(pGraph);

    return SUCC;
}

int GraphRunner::Start()
{
    //TODO：目前thread pool的任务数量不限制，限制的话，submit和run op可能会导致互相等待的问题
    //TODO：需要在sumbit上单独进行限制
    mpThreadPool.reset(new ThreadPool(mNThreads, std::numeric_limits<int>::max()));

    return SUCC;
}

int GraphRunner::Stop()
{
    mpThreadPool.reset();

    return SUCC;
}

int GraphRunner::Wait()
{
    //TODO: WaitableNumeric Wait
    //TODO: 返回全局的error code
    //TODO；需要wait thread pool的任务执行完吗？

    return SUCC;
}

int GraphRunner::SubmitInternal(const std::vector<std::string>& initNameList,
                                std::vector<std::unique_ptr<IOpData>>& initDataList)
{
    int rtn;

    //创建context
    std::shared_ptr<Context> pContext(new Context());

    //context上针对每个op做初始化
    int opSize = mpGraph->OpSize();
    for (int i = 0; i < opSize; i++)
    {
        int opInputDataSize = mpGraph->GetOpInputDataSize(i);
        std::string opName = mpGraph->GetOpName(i);
        rtn = pContext->InitOpContext(opName, opInputDataSize);
        CHECK_RTN(rtn);
    }

    //TODO: 增加Wait的等待， WaitableNumeric Add opSize

    //当作init op来进行后处理
    RunOpDone(pContext, initNameList, initDataList);

    return SUCC;
}

class GraphRunner::OpExecuteTask : public ThreadPool::Task
{
public:
    OpExecuteTask(GraphRunner& runner, std::shared_ptr<Context>& pContext, const std::string& opName) :
        mRunner(runner), mpContext(pContext), mOpName(opName)
    { }
    virtual ~OpExecuteTask() { }
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

void GraphRunner::RunOpInternal(std::shared_ptr<Context> pContext, const std::string& opName)
{
    int rtn;

    //从graph中获取op信息
    //TODO: 最好在graph runner init的时候就基于graph构建好runner侧的op信息，这样可以自己保证pTask的可用性，目前从graph上获取
    IOpTask* pTask = NULL;
    rtn = mpGraph->GetOp(opName, &pTask);
    if (rtn != SUCC)
    {
        abort();
    }

    //根据op的类型来执行：sync、async
    if (pTask->mOpExecType == OpExecType::kSyncExec)
    {
        SyncOpTaskBase* opTask = (SyncOpTaskBase*)pTask;
        RunSyncOp(pContext, *opTask);
    }
    else if (pTask->mOpExecType == OpExecType::kASyncExec)
    {
        ASyncOpTaskBase* opTask = (ASyncOpTaskBase*)pTask;
        RunASyncOp(pContext, *opTask);
    }
    else
    {
        abort();
    }
}

void GraphRunner::RunSyncOp(std::shared_ptr<Context> pContext, SyncOpTaskBase& opTask)
{
    int rtn;

    int inputNameListSize = opTask.mInputNameList.size();
    int outputNameListSize = opTask.mOutputNameList.size();

    //准备op的input数据，从context中获取
    std::vector<std::unique_ptr<IOpData>> inputList(inputNameListSize);
    for (int i = 0; i < inputNameListSize; i++)
    {
        const void* data = NULL;
        rtn = pContext->GetOpOutputData(opTask.mInputNameList[i], data);
        if (rtn != SUCC)
        {
            abort();
        }

        std::unique_ptr<OpInputData> pData(new OpInputData());
        pData->SetData(data);
        inputList[i] = std::move(pData);
    }

    //op执行
    std::vector<std::unique_ptr<IOpData>> outputList;
    rtn = opTask.Run(inputList, outputList);

    //op执行结果检查
    int outputListSize = outputList.size();
    if (rtn == SUCC && outputNameListSize != outputListSize)
    {
        rtn = ERR;
    }
    if (rtn != SUCC)
    {
        //TODO: 设置全局的error info（atomic<T>），wait的时候返回error code

        //TODO: task run可能会出现类型不对、参数数量不对等问题，需要考虑更好的异常处理
        //TODO: 目前考虑将output data设置为null，后续op都跳过，但是waitable number会-1，最后wait返回错误
        outputList.resize(outputNameListSize);
        for (int i = 0; i < outputNameListSize; i++)
        {
            outputList[i].reset(new NullOpData());
        }
    }

    //op后处理
    RunOpDone(pContext, opTask.mOutputNameList, outputList);
}

void GraphRunner::RunASyncOp(std::shared_ptr<Context> pContext, ASyncOpTaskBase& opTask)
{
    int rtn;

    int inputNameListSize = opTask.mInputNameList.size();

    //准备op的input数据，从context中获取
    std::vector<std::unique_ptr<IOpData>> inputList(inputNameListSize);
    for (int i = 0; i < inputNameListSize; i++)
    {
        const void* data = NULL;
        rtn = pContext->GetOpOutputData(opTask.mInputNameList[i], data);
        if (rtn != SUCC)
        {
            abort();
        }

        std::unique_ptr<OpInputData> pData(new OpInputData());
        pData->SetData(data);
        inputList[i] = std::move(pData);
    }

    //op callback
    //TODO：由于是callback，这里需要注意内存的问题，callback在被调用前，涉及的数据和callback不会释放掉
    //TODO：this、pContext、opTask目前看都不会有问题，这里需要测试下有没有问题
    //TODO：callback需要在最后关联到给用户的op callback上，这里需要测试下有没有问题
    //TODO：可能会出现callback已经执行完成，但是当前线程逻辑还没有执行完，会有什么影响需要再思考下
    auto callback =
        [this, pContext, &opTask]
        (int rtn, std::unique_ptr<std::vector<std::unique_ptr<IOpData>>>&& pResult) -> void
        {
            std::unique_ptr<std::vector<std::unique_ptr<IOpData>>> pOutputList = std::move(pResult);

            //op执行结果检查
            int outputNameListSize = opTask.mOutputNameList.size();
            int outputListSize = pOutputList->size();
            if (rtn == SUCC && outputNameListSize != outputListSize)
            {
                rtn = ERR;
            }
            if (rtn != SUCC)
            {
                //TODO: 设置全局的error info（atomic<T>），wait的时候返回error code

                //TODO: task run可能会出现类型不对、参数数量不对等问题，需要考虑更好的异常处理
                //TODO: 目前考虑将output data设置为null，后续op都跳过，但是waitable number会-1，最后wait返回错误
                pOutputList->resize(outputNameListSize);
                for (int i = 0; i < outputNameListSize; i++)
                {
                    pOutputList->at(i).reset(new NullOpData());
                }
            }

            //op后处理
            this->RunOpDone(pContext, opTask.mOutputNameList, *(pOutputList.get()));
        };

    //op执行
    opTask.Run(inputList, callback);
}

void GraphRunner::RunOpDone(std::shared_ptr<Context> pContext,
             const std::vector<std::string>& outputNameList,
             std::vector<std::unique_ptr<IOpData>>& outputList)
{
    int rtn;

    int outputNameListSize = outputNameList.size();
    for (int i = 0; i < outputNameListSize; i++)
    {
        const std::string& outputName = outputNameList[i];

        //context上更新op执行结果
        rtn = pContext->SetOpOutputData(outputName, std::move(outputList[i]));
        if (rtn != SUCC)
        {
            abort();
        }

        //更新当前op关联的后续op的状态，如果input都已经ready，则创建task执行
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
            if (pContext->IncrAndCheckOpContext(nextOpName))
            {
                rtn = mpThreadPool->Submit(std::make_shared<OpExecuteTask>(*this, pContext, nextOpName));
                if (rtn != SUCC)
                {
                    abort();
                }
            }
        }
    }

    //TODO: 当前op执行完毕，通知Wait， WaitableNumeric - 1
}

}
