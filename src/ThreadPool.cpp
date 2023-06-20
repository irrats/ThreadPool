#include <thread> 
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <stdlib.h> 

#include "ThreadPool.hpp"

namespace threadpool
{
    class StopTask: public ITask
    {
        public:
            StopTask(ThreadMap &map_arg, WaitableQueue<std::shared_ptr<WorkerThread> > &wq_arg): m_workingThreads(map_arg), m_availableThreads(wq_arg){}
            void Execute()
            {
                ThreadMap::iterator iter;
                iter = m_workingThreads.Find(std::this_thread::get_id());
                iter->second->SwitchState();
                m_availableThreads.Push(iter->second);
                m_workingThreads.Remove(iter);
            }

        private:
            ThreadMap &m_workingThreads;
            WaitableQueue<std::shared_ptr<WorkerThread> > &m_availableThreads;
    };

    class PauseTask: public ITask
    {
        public:
            PauseTask(std::mutex &mu, std::condition_variable &con, std::atomic<bool> &m_pause): mu_mutex(mu) , cond_var(con), m_paused(m_pause){};
            void Execute()
            {
                while(m_paused) // from  meyers book double check locking pattern 
                {
                    std::unique_lock<std::mutex> lock(mu_mutex);
                    while(m_paused)
                    {
                        cond_var.wait(lock);
                    }
                }  
            }

        private:
            std::mutex &mu_mutex;
            std::condition_variable &cond_var;
            std::atomic<bool> &m_paused;
    };

        ThreadPool::ThreadPool(std::size_t numOfThreads): m_numOfThreads(numOfThreads), m_paused(false), wrap_for_func(std::bind(&ThreadPool::GetTask, this))
    {
        if(0 == numOfThreads)
        {
            m_numOfThreads = 5;
        }
        CreateAndPushThread(m_numOfThreads);
    }

    ThreadPool::~ThreadPool()
    {
        Resume();
        Stop(m_numOfThreads);
    }

    void ThreadPool::Stop(size_t times)
    {
        for (size_t i = 0; i < times; ++i)
        {
            m_Tasks.Push(TaskPair(std::shared_ptr<StopTask>(new StopTask(m_workingThreads, m_availableThreads)), ThreadPool::TASK));
        }

        for (size_t i = 0; i < times; ++i)
        {
           Join();
        } 
    }

    void ThreadPool::Join()
    {
        std::shared_ptr<WorkerThread> work_thread;
        m_availableThreads.Pop(work_thread);
        work_thread->JoinThread();
    }

    void ThreadPool::AddTask(std::shared_ptr<ITask> task, TaskPriority taskPriority_)
    {
        m_Tasks.Push(TaskPair(task, taskPriority_));
    }

    std::shared_ptr<ITask> ThreadPool::GetTask()
    {
        TaskPair task_pair;
        m_Tasks.Pop(task_pair);
        return task_pair.first;
    }

    void ThreadPool::Pause()
    {
        m_paused = true;  

        for (size_t i = 0; i < m_numOfThreads; ++i)
        {
            m_Tasks.Push(TaskPair(std::shared_ptr<PauseTask>(new PauseTask(m_mutex, m_conditon_var, m_paused)), ThreadPool::TASK));  
        }
    }

    void ThreadPool::Resume()
    {   
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_paused = false;
        }
        m_conditon_var.notify_all();
    }

    void ThreadPool::SetThreadNum(size_t n_)
    {
        int check = n_ - m_numOfThreads;

        if(check < 0)
        {
            Stop(abs(check));
        }
        if(check > 0)
        {
            CreateAndPushThread(check);
        }
        this->m_numOfThreads = n_;
    }

    void ThreadPool::CreateAndPushThread(size_t times)
    {
        for(size_t i = 0; i < times; ++i)
        {
            WorkerThread *work_thread = new WorkerThread(wrap_for_func);
            m_workingThreads.Insert({work_thread->GetTID(), std::shared_ptr<WorkerThread>(work_thread)});
        }
    }

    bool operator<(const ThreadPool::TaskPair &lhs, const ThreadPool::TaskPair &rhs)
    {
        return lhs.second < rhs.second;
    }

}