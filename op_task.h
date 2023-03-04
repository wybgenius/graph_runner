#ifndef GRAPHRUNNER_OP_TASK_H_
#define GRAPHRUNNER_OP_TASK_H_

namespace graphrunner
{

class IOpTask:
{
public:


    //Run function

private:

};

template <typename OutputType, typename... InputTypes>
class OpTask<OutputType, InputTypes...> : public IOpTask
{
public:


    //Run function override

private:

};

}

#endif
