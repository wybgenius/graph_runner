#ifndef GRAPHRUNNER_GRAPH_H_
#define GRAPHRUNNER_GRAPH_H_

#include <string>
#include <vector>
#include <functional>

#include "error_code.h"
#include "op_base.h"
#include "op_output.h"
#include "op_task.h"

namespace graphrunner
{

class Graph
{
public:
    Graph() { }

    ~Graph() { }

    //TODO: Input可变参数在前和Output在后怎么搞？目前直接调整会编译不过，应该是加下tuple？
    template <typename OutputType, typename... InputTypes>
    int AddOp(
            const std::vector<std::string>& inputNameList,
            const std::string& outputName,
            const std::string& opName,
            const std::function<int(OpOutput<OutputType>&,
                                    const InputTypes*...)>& func);

    int GetOp(
            const std::string& opName,
            std::shared_ptr<IOpTask>& pTask) { return SUCC; }

    int Build() { return SUCC; }

    int OpSize() { return 0; }

    std::string GetOpName(int index) { return ""; }

    int GetOpInputDataSize(int index) { return 0; }

    int GetOpNameListByInputName(
            const std::string& inputDataName,
            std::vector<std::string>& opNameList)
    { return SUCC; }

};

template <typename OutputType, typename... InputTypes>
int Graph::AddOp(
        const std::vector<std::string>& inputNameList,
        const std::string& outputName,
        const std::string& opName,
        const std::function<int(OpOutput<OutputType>&,
                                const InputTypes*...)>& func)
{
    return SUCC;
}

}

#endif
