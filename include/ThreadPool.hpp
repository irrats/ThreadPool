#pragma once

#include <thread> 
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <stdlib.h> 

#include "WaitableQueue.hpp"
#include "PriorityQueue.hpp"
#include "ITask.hpp"
#include "ThreadMap.hpp"

namespace threadpool
{   
    // class StopTask: public ITask
    // {
    //     public:
    //         StopTask(ThreadMap &map_arg, WaitableQueue<std::shared_ptr<WorkerThread> > &wq_arg): m_workingThreads(map_arg), m_availableThreads(wq_arg){}
    //         void Execute()
    //         {
    //             ThreadMap::iterator iter;
    //             iter = m_workingThreads.Find(std::this_thread::get_id());
    //             iter->second->SwitchState();
    //             m_availableThreads.Push(iter->second);
    //             m_workingThreads.Remove(iter);
    //         }

    //     private:
    //         ThreadMap &m_workingThreads;
    //         WaitableQueue<std::shared_ptr<WorkerThread> > &m_availableThreads;
    // };

    // class PauseTask: public ITask
    // {
    //     public:
    //         PauseTask(std::mutex &mu, std::condition_variable &con, std::atomic<bool> &m_pause): mu_mutex(mu) , cond_var(con), m_paused(m_pause){};
    //         void Execute()
    //         {
    //             while(m_paused) // from  meyers book double check locking pattern 
    //             {
    //                 std::unique_lock<std::mutex> lock(mu_mutex);
    //                 while(m_paused)
    //                 {
    //                     cond_var.wait(lock);
    //                 }
    //             }  
    //         }

    //     private:
    //         std::mutex &mu_mutex;
    //         std::condition_variable &cond_var;
    //         std::atomic<bool> &m_paused;
    // };

    class ThreadPool
    {
        public:
            enum TaskPriority {PRIORITY_LOW = 0, PRIORITY_NORMAL = 1, PRIORITY_HIGH = 2};

            typedef std::pair<std::shared_ptr<ITask>, TaskPriority> TaskPair;
            typedef PriorityQueue<TaskPair, std::vector<TaskPair>, std::less<TaskPair> > TaskPQ;

            explicit ThreadPool(std::size_t numOfThreads = std::thread::hardware_concurrency()); 
            
            ThreadPool(ThreadPool &&) = delete;
            ThreadPool(const ThreadPool &) = delete;
            ThreadPool &operator=(ThreadPool &&) = delete;
            ThreadPool &operator=(const ThreadPool &) = delete;
 
            ~ThreadPool();

            void AddTask(std::shared_ptr<ITask> task, TaskPriority taskPriority_ = PRIORITY_NORMAL);
            void Pause(void);
            void Resume(void);
            void SetThreadNum(std::size_t n_);

        private:

            void Join();
            void Stop(size_t times);
            void CreateAndPushThread(size_t times);

            std::size_t m_numOfThreads;

            std::mutex m_mutex;
            std::condition_variable m_conditon_var;

            ThreadMap m_workingThreads;
            WaitableQueue<TaskPair, TaskPQ> m_Tasks;
            WaitableQueue<std::shared_ptr<WorkerThread> > m_availableThreads;

            std::atomic<bool> m_paused;
            std::shared_ptr<ITask> GetTask();

            std::function<std::shared_ptr<ITask>()>  wrap_for_func;

            TaskPriority TASK = static_cast<TaskPriority>(3);
    };


}


