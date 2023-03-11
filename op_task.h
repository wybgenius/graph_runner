#ifndef GRAPHRUNNER_OP_TASK_H_
#define GRAPHRUNNER_OP_TASK_H_

#include <functional>

#include "error_code.h"
#include "op_base.h"

namespace graphrunner
{

template <typename OutputType, typename... InputTypes>
class SyncOpTask : public SyncOpTaskBase
{
public:
    SyncOpTask(const std::string& opName,
               const std::vector<std::string>& inputNameList,
               const std::string& outputName)
    { }

    virtual ~SyncOpTask() { }

    virtual int Run(std::unique_ptr<IOpDataList>&& pInputList,
                    std::vector<std::unique_ptr<IOpData>>& outputList) override
    { return SUCC; }

private:

};

}

#endif
