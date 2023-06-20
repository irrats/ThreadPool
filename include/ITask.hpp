#pragma once

namespace ilrd
{

class ITask
{
public:
    explicit ITask() = default;
    explicit ITask(ITask &&) = delete;
    explicit ITask(const ITask &) = delete;
    ITask &operator=(ITask &&) = delete;
    ITask &operator=(const ITask &) = delete;
    virtual ~ITask() = default;

    virtual void Execute() = 0;
};

}

