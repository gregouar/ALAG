#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#include <iostream>
#include <mutex>
#include <atomic>
#include "ALAGE/utils/SingletonsCleaner.h"

/*template<class T> class Singleton
{
public:

    static T* Instance(void)
    {
        return &m_singleton;
    }


protected:

    Singleton(){}
    virtual ~Singleton(){}

    static T m_singleton;
};

template <class T> T Singleton<T>::m_singleton=T();*/


template<typename T> class Singleton : public KillableSingleton
{
    public:

        static T* Instance()
        {
            /*if(m_singleton == nullptr)
            {
                m_singleton = new T;
                SingletonsCleaner::AddToList(m_singleton);
            }
            return m_singleton;*/
            T* tmp = m_instance.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            if (tmp == nullptr) {
                std::lock_guard<std::mutex> lock(m_mutex);
                tmp = m_instance.load(std::memory_order_relaxed);
                if (tmp == nullptr) {
                    tmp = new T;
                    std::atomic_thread_fence(std::memory_order_release);
                    m_instance.store(tmp, std::memory_order_relaxed);
                    SingletonsCleaner::AddToList(tmp);
                }
            }
            return tmp;
        }

       /* static Tp &
instance()
{
    static thread_local Tp *instance;

    if (!instance &&
        !(instance = m_instance.load(std::memory_order_acquire)))
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!(instance = m_instance.load(std::memory_order_relaxed)))
        {
            instance = new Tp;
            m_instance.store(instance, std::memory_order_release);
        }
    }
    return *instance;
}*/

    protected:

        Singleton(){}
        virtual ~Singleton(){}

        static std::atomic<T*> m_instance;
        static std::mutex m_mutex;

    private:

        virtual void Kill()
        {
            T* tmp = m_instance.load(std::memory_order_relaxed);
            if(tmp != nullptr)
            {
                //std::lock_guard<std::mutex> lock(m_mutex);
                SingletonsCleaner::RemoveFromList(tmp);
                delete tmp;
                m_instance.store(nullptr);
            }
        }
};

//template <typename T>  T* Singleton<T>::m_instance = nullptr;
template <typename T> std::atomic<T*> Singleton<T>::m_instance;
template <typename T> std::mutex Singleton<T>::m_mutex;


#endif // SINGLETON_H_INCLUDED
