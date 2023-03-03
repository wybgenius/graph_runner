#ifndef GRAPHRUNNER_OP_OUTPUT_H_
#define GRAPHRUNNER_OP_OUTPUT_H_

namespace graphrunner
{

class IOpOutput
{

};

template <typename OutputType>
class OpOutput : public IOpOutput
{
public:
    OpOutput() { }

    ~OpOutput()
    {
        if (mDataAutoDelete)
        {
            delete mpData;
            mDataAutoDelete = false;
        }
        mpData = NULL;
    }

    void SetNewData(OutputType* pOutput)
    {
        mpData = pOutput;
        mDataAutoDelete = true;
    }

    void SetNewData(std::unique_ptr<OutputType>&& pOutput)
    {
        mpData = pOutput.release();
        mDataAutoDelete = true;
    }

    void SetExistData(const OutputType* pOutput)
    {
        mpData = pOutput;
        mDataAutoDelete = true;
    }

    OutputType* GetRawData() { return mpData; }

private:
    OutputType* mpData;
    bool mDataAutoDelete;
};

}

#endif
