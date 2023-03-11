#ifndef GRAPHRUNNER_OP_BASE_H_
#define GRAPHRUNNER_OP_BASE_H_

namespace graphrunner
{

class IOpTaskVisitor;

class IOpTask
{
public:
    IOpTask(const std::string& opName,
            const std::vector<std::string>& inputNameList,
            const std::vector<std::string>& outputNameList) :
        mOpName(opName), mInputNameList(inputNameList),
        mOutputNameList(outputNameList)
    { }
    virtual ~IOpTask() { }
    virtual void Accept(IOpTaskVisitor& visitor) = 0;

    const std::string mOpName;
    const std::vector<std::string> mInputNameList;
    const std::vector<std::string> mOutputNameList;
};

class IOpDataList
{
public:
    virtual ~IOpDataList() { }
    virtual int Size() const = 0;
    virtual const void* GetData(int i) const = 0;
};

class IOpData
{
public:
    virtual ~IOpData() { }
    virtual const void* GetData() const = 0;
};

class SyncOpTaskBase;

class IOpTaskVisitor
{
public:
    virtual ~IOpTaskVisitor() { }
    virtual void Visit(SyncOpTaskBase& opTask) = 0;
    //virtual void Visit(ASyncOpTaskBase& opTask) = 0;
};

class SyncOpTaskBase : public IOpTask
{
public:
    SyncOpTaskBase(const std::string& opName,
            const std::vector<std::string>& inputNameList,
            const std::vector<std::string>& outputNameList) :
        IOpTask(opName, inputNameList, outputNameList)
    { }

    virtual ~SyncOpTaskBase() { }

    virtual void Accept(IOpTaskVisitor& visitor) override
    {
        visitor.Visit(*this);
    }

    virtual int Run(std::unique_ptr<IOpDataList>&& pInputList,
                    std::vector<std::unique_ptr<IOpData>>& outputList) = 0;
};

//class ASyncOpTaskBase;

}

#endif
