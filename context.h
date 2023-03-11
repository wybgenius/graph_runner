#ifndef GRAPHRUNNER_CONTEXT_H_
#define GRAPHRUNNER_CONTEXT_H_

#include <string>
#include <functional>

#include "error_code.h"
#include "op_base.h"

namespace graphrunner
{

class Context
{
public:
    Context() { }

    ~Context() { }

    int InitOpInputDataContext(const std::string& opName,
                               int opInputDataSize) { return SUCC; }

    int IncrAndCheckOpInputDataContext(const std::string& opName) { return SUCC; }

    int SetOpOutputData(const std::string& outputDataName,
                        std::unique_ptr<IOpData>&& outputData) { return SUCC; }

    const void* GetOpOutputData(const std::string& outputDataName) { return NULL; }

private:
    // lock?

    // map: op name -> op context info(input data size, current input data ready size)

    // map: op output name -> op output data(OpOutput)
};

}

#endif
