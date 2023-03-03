#ifndef GRAPHRUNNER_OP_INPUT_H_
#define GRAPHRUNNER_OP_INPUT_H_

#include <vector>

namespace graphrunner
{

class IOpInput
{
public:
    IOpInput(int size)
    {
        mSize = size;

        mDataList.resize(size);
    }

    ~IOpInput() { }

    int Size() { return mSize; }

    void SetData(int i, void* pData) { mDataList[i] = pData; }

    void* GetData(int i) { return mDataList[i]; }

private:
    int mSize;

    std::vector<void*> mDataList;
};

}

#endif
