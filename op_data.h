#ifndef GRAPHRUNNER_OP_DATA_H_
#define GRAPHRUNNER_OP_DATA_H_

#include <vector>
#include <functional>

#include "op_base.h"

namespace graphrunner
{

class OpInputDataList : public IOpDataList
{
public:
    OpInputDataList(int size)
    {
        mSize = size;

        mDataList.resize(size);
    }

    virtual ~OpInputDataList() { }

    virtual int Size() const override { return mSize; }

    virtual const void* GetData(int i) const override { return mDataList[i]; }

    void SetData(int i, const void* pData) { mDataList[i] = pData; }

private:
    int mSize;

    std::vector<const void*> mDataList;
};

template <typename Type>
class UniqPtrOpData : public IOpData
{
public:
    UniqPtrOpData() { }
    
    virtual ~UniqPtrOpData() { }

    virtual const void* GetData() const override
    {   
        return mpData.get();
    }

    void SetData(std::unique_ptr<Type>&& pData)
    {   
        mpData = std::move(pData);
    }

private:
    std::unique_ptr<Type> mpData;
};

template <typename Type>
class OpOutputData : public IOpData
{
public:
    OpOutputData() { }
    
    virtual ~OpOutputData()
    {   
        if (mDataAutoDelete)
        {   
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
        mDataAutoDelete = true;
    }

private:
    Type* mpData;
    bool mDataAutoDelete;
};

}

#endif
