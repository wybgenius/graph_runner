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
    GraphRunner(std::unique_ptr<Graph>&& pGraph,
                int nThreads,
                int maxTaskNum);

    virtual ~GraphRunner();

    int Start();
    int Stop();

    template <typename Type>
    int Submit(std::unique_ptr<Type>&& pInput);

    int Wait();

private:
    int SubmitInternal(const std::vector<std::string>& outputNameList,
                       std::vector<std::unique_ptr<IOpData>>& inputList);

    class ExecuteTask;

    class OpTaskVisitor;

    void RunOpInternal(std::shared_ptr<Context>& pContext, const std::string& opName);

    void RunSyncOp(std::shared_ptr<Context>& pContext, SyncOpTaskBase& opTask);

    //void RunASyncOp(std::shared_ptr<Context>& pContext, ASyncOpTaskBase& opTask);

    void RunOpDone(std::shared_ptr<Context>& pContext,
                 const std::vector<std::string>& outputNameList,
                 std::vector<std::unique_ptr<IOpData>>& outputList);

private:
    std::unique_ptr<Graph> mpGraph;

    std::unique_ptr<ThreadPool> mpThreadPool;
};


template <typename Type>
int GraphRunner::Submit(std::unique_ptr<Type>&& pInput)
{
    std::unique_ptr<UniqPtrOpData<Type>> opInputData(new UniqPtrOpData<Type>());
    opInputData->SetData(std::move(pInput));

    std::vector<std::string> initNameList;
    initNameList.emplace_back(Constants::sGraphRunnerInitInputName);

    std::vector<std::unique_ptr<IOpData>> initDataList;
    initDataList.emplace_back(std::move(opInputData));

    return SubmitInternal(initNameList, initDataList);
}

}

#endif
