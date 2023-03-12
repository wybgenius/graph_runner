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
#include "op_base.h"
#include "op_data.h"

namespace graphrunner
{

class GraphRunner
{
public:
    GraphRunner(int nThreads);

    virtual ~GraphRunner();

    //根据graph构建runner自己的结构，目前直接用的graph信息
    int Init(std::unique_ptr<Graph>&& pGraph);

    int Start();
    int Stop();

    //runner上提交数据，目前对外先只支持单输入
    template <typename Type>
    int Submit(std::unique_ptr<Type>&& pInput);

    //等待runner上的所有提交的数据对应的op任务都执行完
    int Wait();

private:
    //submit的内部执行入口，支持多输入
    int SubmitInternal(const std::vector<std::string>& initNameList,
                       std::vector<std::unique_ptr<IOpData>>& initDataList);

    //op在thread pool上执行的task
    class OpExecuteTask;

    //op的内部执行入口
    void RunOpInternal(std::shared_ptr<Context> pContext, const std::string& opName);

    //同步op的执行入口
    void RunSyncOp(std::shared_ptr<Context> pContext, SyncOpTaskBase& opTask);

    //异步op的执行入口
    void RunASyncOp(std::shared_ptr<Context> pContext, ASyncOpTaskBase& opTask);

    //op执行完成的后处理
    void RunOpDone(std::shared_ptr<Context> pContext,
                 const std::vector<std::string>& outputNameList,
                 std::vector<std::unique_ptr<IOpData>>& outputList);

private:
    std::unique_ptr<Graph> mpGraph;

    int mNThreads;
    std::unique_ptr<ThreadPool> mpThreadPool;
};

template <typename Type>
int GraphRunner::Submit(std::unique_ptr<Type>&& pInput)
{
    std::unique_ptr<OpInitData<Type>> opInputData(new OpInitData<Type>());
    opInputData->SetData(std::move(pInput));

    std::vector<std::string> initNameList;
    initNameList.emplace_back(Constants::sGraphRunnerInitInputName);

    std::vector<std::unique_ptr<IOpData>> initDataList;
    initDataList.emplace_back(std::move(opInputData));

    return SubmitInternal(initNameList, initDataList);
}

}

#endif
