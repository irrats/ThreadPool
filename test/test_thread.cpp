#include <iostream>
#include <unistd.h>

#include "ThreadPool.hpp"

using namespace threadpool;
using namespace std;

class MyTask : public ITask
{
public:
    void Execute() override
    {
        cout << "Mytask" << endl;
    }
};

int main()
{

    for (int i = 0; i < 2; ++i)
    {
        ThreadPool tp(8);
        shared_ptr<ITask> ptr(new MyTask());

        tp.AddTask(ptr);
        tp.SetThreadNum(5);
        sleep(2);
        tp.AddTask(ptr);
        tp.AddTask(ptr);
        tp.SetThreadNum(10);

        tp.AddTask(ptr);
        sleep(1);
        tp.Pause();
        sleep(1);
        tp.Resume();
        sleep(1);
        tp.AddTask(ptr);
        tp.AddTask(ptr);
        tp.AddTask(ptr);
        tp.AddTask(ptr);
        tp.AddTask(ptr);
        sleep(1);
    }
    cout << "bye" << endl;

    return 0;
}
