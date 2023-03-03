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

void GraphRunner::RunInternal(std::shared_ptr<Context>& context, std::string& opName)
{
    //TODO: 从graph中获取op的信息, IOpTask, op input data name list, op output data name
    //TODO：IOpTask需要再AddOp的时候就处理好
    //TODO: IOpOutput的生成需要思考下，应该是在AddOp的时候，先搞一个IOpOutputCreator，这里生成IOpOutput


    //TODO: 从context中，根据op input data name list，从output data中拿出来IOpOutput，然后根据顺序生成IOpInput


    //TODO: op task run, IOpTask::Run(IOpInput, IOpOutput)


    //TODO: 把opOutput写入context中


    //TODO: WaitableNumeric - 1


    //TODO: 通过GetOpNameListByInputName(op output data name)来获取需要更新状态的op


    //TODO: 针对需要更新状态的op调用IncrAndCheck来检查op input data的情况，如果true，说明该op的input data已经ready
    //TODO: 这里的并发问题需要思考下


    //TODO: 生成该op的op task，然后执行
    //TODO: 这里需要细考虑，直接加到thread pool应该会有死锁问题，buffer设置小的情况下，都会阻塞住，然后又消费不掉
    //TODO: 这里看起来不能用opExecFunc，看起来需要写个内部的Task类

}

}
