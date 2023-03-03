#ifndef GRAPHRUNNER_OBJECT_BUFFER_H_
#define GRAPHRUNNER_OBJECT_BUFFER_H_

#include <memory>
#include <string>

namespace graphrunner
{

template<typename T>
class ObjectBuffer
{
public:
    ObjectBuffer(const std::string& name, unsigned int maxSize, unsigned int nProducer = 0, unsigned int nConsumer = 0) { }

    virtual ~ObjectBuffer() { }

    int Push(const T& t);
    int Push(T&& t);

    int TryPush(const T& t);
    int TryPush(T&& t);

    int ProducerQuit(bool mute = false);

    int Pop(T& t);

    int TryPop(T& t);

    int Clear();

    int ConsumerQuit();

    unsigned int Size() const;

    unsigned int MaxSize() const;

    unsigned int GetProducerNum() const;

private:
    //struct Impl;
    //std::unique_ptr<Impl> mpImpl;
};

}

#endif
