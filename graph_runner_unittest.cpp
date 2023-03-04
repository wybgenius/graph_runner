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
    int count = 0;
    std::unique_ptr<Graph> pGraph(new Graph());

    std::vector<std::string> inputNameList;
   
    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    std::function<int(OpOutput<int>&,
                      const std::string*)> op1 = 
        [&count](OpOutput<int>& output,
                 const std::string* pInit) -> int
        {
            EXPECT_EQ(*pInit, "abc");
            output.SetNewData(new int(123));

            count++;
            return SUCC;
        };
    rtn = pGraph->AddOp<int, std::string>(inputNameList, "output1", "op1", op1);
    EXPECT_EQ(rtn, SUCC);

    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    std::function<int(OpOutput<double>&,
                      const std::string*)> op2 = 
        [&count](OpOutput<double>& output,
                 const std::string* pInit) -> int
        {
            EXPECT_EQ(*pInit, "abc");
            output.SetNewData(new double(0.123));

            count++;
            return SUCC;
        };
    rtn = pGraph->AddOp<double, std::string>(inputNameList, "output2", "op2", op2);
    EXPECT_EQ(rtn, SUCC);

    inputNameList = { "output1", "output2" };
    std::function<int(OpOutput<int>&,
                      const int*,
                      const double*)> op3 = 
        [&count](OpOutput<int>& output,
                 const int* pInput1,
                 const double* pInput2) -> int
        {
            EXPECT_EQ(*pInput1, 123);
            EXPECT_EQ(*pInput2, 0.123);
            output.SetNewData(new int(345));

            count++;
            return SUCC;
        };
    rtn = pGraph->AddOp<int, int, double>(inputNameList, "output3", "op3", op3);
    EXPECT_EQ(rtn, SUCC);

    std::unique_ptr<GraphRunner> pRunner(new GraphRunner(std::move(pGraph), 10, 1024));
    pRunner->Start();
    rtn = pRunner->Submit<std::string>(
            std::unique_ptr<std::string>(new std::string("abc")));
    EXPECT_EQ(rtn, SUCC);
    pRunner->Wait();
    pRunner->Stop();

    EXPECT_EQ(count, 3);
}

}
