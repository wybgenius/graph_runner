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

class SyncTestOp
{
public:
    SyncTestOp(int* count) : mCount(count) { }
    ~SyncTestOp() { }

    int Init() { return SUCC; }

    int operator()(const std::string* pInit,
                   OpOutput<int>& output) const
    {
        EXPECT_EQ(*pInit, "abc");
        output.SetNewData(new int(123));

        (*mCount)++;

        return SUCC;
    }
private:
    int* mCount;
};

TEST(GraphRunnerNormalUnitTest, SyncOpUnitTest1)
{
    int rtn(SUCC); 
    int count = 0;

    std::unique_ptr<Graph> pGraph(new Graph());

    std::vector<std::string> inputNameList;

    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    SyncTestOp op1(&count);
    op1.Init();
    rtn = pGraph->AddOp<std::string, int>(inputNameList, "output1", "op1", op1);
    EXPECT_EQ(rtn, SUCC);
 
    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    auto op2 =
        [&count](const std::string* pInit,
                 OpOutput<double>& output) -> int
        {
            EXPECT_EQ(*pInit, "abc");
            output.SetNewData(new double(0.123));

            count++;
            return SUCC;
        };
    rtn = pGraph->AddOp<std::string, double>(inputNameList, "output2", "op2", op2);
    EXPECT_EQ(rtn, SUCC);

    inputNameList = { "output1", "output2" };
    auto op3 =
        [&count](const int* pInput1,
                 const double* pInput2,
                 OpOutput<int>& output) -> int
        {
            EXPECT_EQ(*pInput1, 123);
            EXPECT_EQ(*pInput2, 0.123);
            output.SetNewData(new int(345));

            count++;
            return SUCC;
        };
    rtn = pGraph->AddOp<int, double, int>(inputNameList, "output3", "op3", op3);
    EXPECT_EQ(rtn, SUCC);

    rtn = pGraph->Build();
    EXPECT_EQ(rtn, SUCC);

    std::unique_ptr<GraphRunner> pRunner(new GraphRunner(10));
    rtn = pRunner->Init(std::move(pGraph));
    EXPECT_EQ(rtn, SUCC);
    pRunner->Start();
    rtn = pRunner->Submit<std::string>(
            std::unique_ptr<std::string>(new std::string("abc")));
    EXPECT_EQ(rtn, SUCC);
    rtn = pRunner->Wait();
    EXPECT_EQ(rtn, SUCC);
    pRunner->Stop();

    EXPECT_EQ(count, 3);
}

class ASyncTestOp
{
public:
    ASyncTestOp(int* count) : mCount(count) { }
    ~ASyncTestOp() { }

    int Init() { return SUCC; }

    void operator()(const std::string* pInit,
                    const Graph::Callback<int>& callback) const
    {
        EXPECT_EQ(*pInit, "abc");

        (*mCount)++;

        callback(SUCC, std::unique_ptr<int>(new int(123)));
    }
private:
    int* mCount;
};

TEST(GraphRunnerNormalUnitTest, ASyncOpUnitTest1)
{
    int rtn(SUCC);
    int count = 0;

    std::unique_ptr<Graph> pGraph(new Graph());

    std::vector<std::string> inputNameList;

    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    ASyncTestOp aop1(&count);
    aop1.Init();
    rtn = pGraph->AddOp<std::string, int>(inputNameList, "output1", "aop1", aop1);
    EXPECT_EQ(rtn, SUCC);

    inputNameList = {
        Constants::sGraphRunnerInitInputName
    };
    //TODO：callback中加timer来进行测试
    auto aop2 =
        [&count](const std::string* pInit,
                 const Graph::Callback<int>& callback)
        {
            EXPECT_EQ(*pInit, "abc");

            count++;

            callback(SUCC, std::unique_ptr<int>(new int(123)));
        };
    rtn = pGraph->AddOp<std::string, int>(inputNameList, "output2", "aop2", aop2);
    EXPECT_EQ(rtn, SUCC);

    rtn = pGraph->Build();
    EXPECT_EQ(rtn, SUCC);

    std::unique_ptr<GraphRunner> pRunner(new GraphRunner(10));
    rtn = pRunner->Init(std::move(pGraph));
    EXPECT_EQ(rtn, SUCC);
    pRunner->Start();
    rtn = pRunner->Submit<std::string>(
            std::unique_ptr<std::string>(new std::string("abc")));
    EXPECT_EQ(rtn, SUCC);
    rtn = pRunner->Wait();
    EXPECT_EQ(rtn, SUCC);
    pRunner->Stop();

    EXPECT_EQ(count, 1);
}

}
