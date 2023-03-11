#ifndef GRAPHRUNNER_OP_OUTPUT_H_
#define GRAPHRUNNER_OP_OUTPUT_H_

#include <functional>

#include "op_data.h"

namespace graphrunner
{

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

}

#endif
