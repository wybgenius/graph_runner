#ifndef GRAPHRUNNER_OP_DATA_H_
#define GRAPHRUNNER_OP_DATA_H_

#include <vector>
#include <functional>

#include "op_base.h"

namespace graphrunner
{

//graph runner submit单个输入数据的类型
template <typename Type>
class OpInitData : public IOpData
{
public:
    OpInitData() { }
    
    virtual ~OpInitData() { }

    virtual const void* GetData() const override
    {   
        return mpData.get();
    }

    void SetData(std::unique_ptr<Type>&& pData)
    {   
        mpData = std::move(pData);
    }

    void Reset()
    {
        mpData.reset();
    }

private:
    std::unique_ptr<Type> mpData;
};

//op单个输入数据的类型
class OpInputData : public IOpData
{
public:
    OpInputData() { }

    virtual ~OpInputData() { }

    virtual const void* GetData() const override
    {
        return mpData;
    }

    void SetData(const void* pData)
    {
        mpData = pData;
    }

private:
    const void* mpData;
};

//op单个输出数据的类型
template <typename Type>
class OpOutputData : public IOpData
{
public:
    OpOutputData() { }
    
    virtual ~OpOutputData()
    {   
        if (mDataAutoDelete)
        {   
            if (mpData)
                delete mpData;
            mDataAutoDelete = false;
        }
        mpData = NULL;
    }
    
    virtual const void* GetData() const override
    {   
        return mpData;
    }
    
    void SetNewData(Type* pOutput)
    {   
        mpData = pOutput; 
        mDataAutoDelete = true;
    }
    
    void SetExistData(const Type* pOutput)
    {   
        mpData = pOutput; 
        mDataAutoDelete = false;
    }

private:
    Type* mpData;
    bool mDataAutoDelete;
};

//op执行异常的情况下，目前通过NULL数据跳过后续op的执行
class NullOpData : public IOpData
{
public:
    NullOpData() { }

    virtual ~NullOpData() { }

    virtual const void* GetData() const override
    {
        return NULL;
    }
};

}

#endif
