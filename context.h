#ifndef GRAPHRUNNER_CONTEXT_H_
#define GRAPHRUNNER_CONTEXT_H_

#include <string>

#include "error_code.h"
#include "op_output.h"

namespace graphrunner
{

class Context
{
public:
    Context() { }

    ~Context() { }

    int InitOpInputDataContext(std::string opName, int opInputDataSize) { return SUCC; }

    int IncrOpInputDataContext(std::string opName) { return SUCC; }

    int SetOpOutputData(std::string outputDataName, std::unique_ptr<IOpOutput>&& outputData) { return SUCC; }

private:
    // lock?

    // map: op name -> op context info(input data size, current input data ready size)

    // map: op output name -> op output data(OpOutput)
};

}

#endif
