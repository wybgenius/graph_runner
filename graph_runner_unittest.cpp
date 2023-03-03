#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <gtest/gtest.h>

#include "graph.h"
#include "graph_runner.h"
#include "constants.h"

namespace graphrunner
{

TEST(GraphRunnerNormalUnitTest, SimpleUnitTest)
{
    int rtn(SUCC); 
    std::unique_ptr<Graph> pGraph(new Graph());

    std::vector<std::string> inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    std::function<int(OpOutput<int>&,
                      const std::string*)> op1 = 
        [](OpOutput<int>& output,
           const std::string* init) -> int
        {
            EXPECT_EQ(*init, "abc");
            output.SetNewData(new int(123));
            return SUCC;
        };
    rtn = pGraph->AddOp<int, std::string>(inputNameList, "output1", "op1", op1);
    EXPECT_EQ(rtn, SUCC);

    std::unique_ptr<GraphRunner> pRunner(new GraphRunner(std::move(pGraph), 10, 1024));
    pRunner->Start();
    rtn = pRunner->Submit<std::string>(
            std::unique_ptr<std::string>(new std::string("abc")));
    EXPECT_EQ(rtn, SUCC);
    pRunner->Wait();
    pRunner->Stop();
}

}
