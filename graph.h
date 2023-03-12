#ifndef GRAPHRUNNER_GRAPH_H_
#define GRAPHRUNNER_GRAPH_H_

#include <string>
#include <vector>
#include <functional>
#include <tuple>

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

    //TODO: input和output可变参数问题，目前在函数上使用可变参数会有解析问题，并且函数对象会不支持

    //添加同步op，这是1输入1输出
    template <typename InputType0, typename OutputType>
    int AddOp(
            const std::vector<std::string>& inputNameList,
            const std::string& outputName,
            const std::string& opName,
            const std::function<int(const InputType0*,
                                    OpOutput<OutputType>&)>& func)
    { return SUCC; }

    //添加同步op，这是1输入2输出
    template <typename InputType0, typename InputType1, typename OutputType>
    int AddOp(
            const std::vector<std::string>& inputNameList,
            const std::string& outputName,
            const std::string& opName,
            const std::function<int(const InputType0*,
                                    const InputType1*,
                                    OpOutput<OutputType>&)>& func)
    { return SUCC; }

    template <typename OutputType>
    using Callback = std::function<void(int, std::unique_ptr<OutputType>&&)>;

    //添加异步op，这是1输入1输出
    template <typename InputType0, typename OutputType>
    int AddOp(
            const std::vector<std::string>& inputNameList,
            const std::string& outputName,
            const std::string& opName,
            const std::function<void(const InputType0*,
                                     const Callback<OutputType>&)>& func)
    { return SUCC; }

    //检查graph的完整性
    int Build() { return SUCC; }

    //获取op执行任务信息，pTask需要保证一直存在
    //TODO: pTask目前提供的方式还需要再思考下
    int GetOp(
            const std::string& opName,
            IOpTask** pTask) { return SUCC; }

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

template <typename InputTypes, typename OutputTypes>
class SyncOpTask;

//同步op具体执行任务的类，支持多输入多输出，在add op的时候需要先生成好
template <typename... InputTypes, typename... OutputType>
class SyncOpTask<std::tuple<InputTypes...>, std::tuple<OutputType...>> : public SyncOpTaskBase
{
public:
    SyncOpTask(const std::string& opName,
               const std::vector<std::string>& inputNameList,
               const std::vector<std::string>& outputNameList) :
        SyncOpTaskBase(opName, inputNameList, outputNameList)
    { }

    virtual ~SyncOpTask() { }

    virtual int Run(std::vector<std::unique_ptr<IOpData>>& inputList,
                    std::vector<std::unique_ptr<IOpData>>& outputList) override
    { return SUCC; }

private:

};

template <typename InputTypes, typename OutputTypes>
class ASyncOpTask;

//异步op具体执行任务的类，支持多输入多输出，在add op的时候需要先生成好
template <typename... InputTypes, typename... OutputType>
class ASyncOpTask<std::tuple<InputTypes...>, std::tuple<OutputType...>> : public ASyncOpTaskBase
{
public:
    ASyncOpTask(const std::string& opName,
               const std::vector<std::string>& inputNameList,
               const std::vector<std::string>& outputNameList) :
        ASyncOpTaskBase(opName, inputNameList, outputNameList)
    { }

    virtual ~ASyncOpTask() { }

    virtual void Run(std::vector<std::unique_ptr<IOpData>>& inputList,
                     const Callback<std::vector<std::unique_ptr<IOpData>>>& callback) override
    { }

private:

};

}

#endif
