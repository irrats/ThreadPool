#pragma once

#include <thread>
#include <functional>
#include <iostream>
#include <memory>

#include "ITask.hpp"

namespace threadpool
{
    class WorkerThread
    {
        public:
            enum State {RUNNING, STOPPED};

            explicit WorkerThread(std::function<std::shared_ptr<ITask>()> fGetTask_);
            
            explicit WorkerThread(WorkerThread &&) = default;
            WorkerThread(const WorkerThread &) = delete;
            WorkerThread &operator=(WorkerThread &&) = default;
            WorkerThread &operator=(const WorkerThread &) = delete;
            ~WorkerThread() = default;

            void Stop();
            State GetState() const;
            std::thread::id GetTID() const;

            void SwitchState();
            void JoinThread();

        private:
            State m_state;
            std::function<std::shared_ptr<ITask>()> m_taskGetter;
            std::thread m_thread;
            void Run();
    };

}


