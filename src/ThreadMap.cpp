#include "ThreadMap.hpp"

namespace threadpool
{

    ThreadMap::iterator ThreadMap:: Find(Key key_)
    {
        std::unique_lock<std::mutex> m(m_mutex);
        return m_threadMap.find(key_);
    }

    void ThreadMap::Insert(const KeyValPair &kvPair_)
    {
        std::unique_lock<std::mutex> m(m_mutex);
        m_threadMap.insert(kvPair_);
    }

    void ThreadMap::Remove(iterator pos)
    {
        std::unique_lock<std::mutex> m(m_mutex);
        m_threadMap.erase(pos);
    }

    WorkerThread::State ThreadMap::GetState(ThreadMap::Key key_) const
    {
        std::unique_lock<std::mutex> m(m_mutex);
        return m_threadMap.find(key_)->second->GetState();
    }
}