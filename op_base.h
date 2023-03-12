#ifndef GRAPHRUNNER_OP_BASE_H_
#define GRAPHRUNNER_OP_BASE_H_

namespace graphrunner
{

//op输入和输出的单个数据的抽象
class IOpData
{
public:
    virtual ~IOpData() { }
    virtual const void* GetData() const = 0;
};

//op的执行类型
enum OpExecType
{
    kSyncExec = 0,
    kASyncExec = 1,
};

//op执行任务的抽象
class IOpTask
{
public:
    IOpTask(const std::string& opName,
            OpExecType opType,
            const std::vector<std::string>& inputNameList,
            const std::vector<std::string>& outputNameList) :
        mOpName(opName), mOpExecType(opType),
        mInputNameList(inputNameList), mOutputNameList(outputNameList)
    { }
    virtual ~IOpTask() { }

    const std::string mOpName;
    OpExecType mOpExecType;
    const std::vector<std::string> mInputNameList;
    const std::vector<std::string> mOutputNameList;
};

//op同步执行任务的抽象
class SyncOpTaskBase : public IOpTask
{
public:
    SyncOpTaskBase(const std::string& opName,
            const std::vector<std::string>& inputNameList,
            const std::vector<std::string>& outputNameList) :
        IOpTask(opName, OpExecType::kSyncExec, inputNameList, outputNameList)
    { }

    virtual ~SyncOpTaskBase() { }

    virtual int Run(std::vector<std::unique_ptr<IOpData>>& inputList,
                    std::vector<std::unique_ptr<IOpData>>& outputList) = 0;
};

//op异步执行任务的抽象
class ASyncOpTaskBase : public IOpTask
{
public:
    ASyncOpTaskBase(const std::string& opName,
            const std::vector<std::string>& inputNameList,
            const std::vector<std::string>& outputNameList) :
        IOpTask(opName, OpExecType::kASyncExec, inputNameList, outputNameList)
    { }

    virtual ~ASyncOpTaskBase() { }
};

}

#endif
