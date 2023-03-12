#ifndef GRAPHRUNNER_GRAPH_H_
#define GRAPHRUNNER_GRAPH_H_

#include <string>
#include <vector>
#include <functional>

#include "error_code.h"
#include "op_base.h"
#include "op_data.h"

namespace graphrunner
{

template <typename OutputType>
class OpOutput;

class Graph
{
public:
    Graph() { }

    ~Graph() { }

    //TODO: Input可变参数在前和Output在后怎么搞？目前直接调整会编译不过，应该是加下tuple？
    //添加op，目前支持多输入单输出
    template <typename OutputType, typename... InputTypes>
    int AddOp(
            const std::vector<std::string>& inputNameList,
            const std::string& outputName,
            const std::string& opName,
            const std::function<int(OpOutput<OutputType>&,
                                    const InputTypes*...)>& func)
    { return SUCC; }

    //检查graph的完整性
    int Build() { return SUCC; }

    //获取op执行任务信息
    int GetOp(
            const std::string& opName,
            std::shared_ptr<IOpTask>& pTask) { return SUCC; }

    //获取graph上的op数量
    int OpSize() { return 0; }

    //根据index获取op名称
    std::string GetOpName(int index) { return ""; }

    //根据index获取op输入的参数数量
    int GetOpInputDataSize(int index) { return 0; }

    //通过op的input name获取op列表
    int GetOpNameListByInputName(
            const std::string& inputDataName,
            std::vector<std::string>& opNameList)
    { return SUCC; }

};

//op输出数据的写入对象
template <typename OutputType>
class OpOutput
{
public:
    OpOutput() : mpData(new OpOutputData<OutputType>()) { }

    virtual ~OpOutput() { }

    void SetNewData(OutputType* pOutput)
    {
        mpData->SetNewData(pOutput);
    }

    void SetNewData(std::unique_ptr<OutputType>&& pOutput)
    {
        mpData->SetNewData(pOutput.release());
    }

    void SetExistData(const OutputType* pOutput)
    {
        mpData->SetExistData(pOutput);
    }

    void GetData(std::unique_ptr<OpOutputData<OutputType>>& pOutput)
    {
        pOutput.reset(mpData.release());
    }

private:
    std::unique_ptr<OpOutputData<OutputType>> mpData;
};

//同步op具体执行任务的类，在add op的时候需要先生成好
template <typename OutputType, typename... InputTypes>
class SyncOpTask : public SyncOpTaskBase
{
public:
    SyncOpTask(const std::string& opName,
               const std::vector<std::string>& inputNameList,
               const std::string& outputName)
    { }

    virtual ~SyncOpTask() { }

    virtual int Run(std::vector<std::unique_ptr<IOpData>>& inputList,
                    std::vector<std::unique_ptr<IOpData>>& outputList) override
    { return SUCC; }

private:

};

}

#endif
