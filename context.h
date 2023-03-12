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

    //初始化op的context，目前保存op的入度信息，用于检查op是否ready
    int InitOpContext(const std::string& opName,
                               int opInputDataSize) { return SUCC; }

    //op context更新操作：op的入度+1，并且检查op是否ready，incr和check需要保证原子
    int IncrAndCheckOpContext(const std::string& opName) { return SUCC; }

    //保存op执行的output结果
    int SetOpOutputData(const std::string& outputDataName,
                        std::unique_ptr<IOpData>&& outputData) { return SUCC; }

    //获取op执行的output结果
    using ConstVoidPtr = const void*;
    int GetOpOutputData(const std::string& outputDataName, ConstVoidPtr& opOutputData) { return SUCC; }

private:
    // lock

    // map: op name -> op context info(input data size, current input data ready size)

    // map: op output name -> op output data(OpOutput)
};

}

#endif
