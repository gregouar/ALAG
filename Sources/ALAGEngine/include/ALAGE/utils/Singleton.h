#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#include <iostream>
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
            if(m_singleton == nullptr)
            {
                m_singleton = new T;
                SingletonsCleaner::AddToList(m_singleton);
            }
            return m_singleton;
        }

        virtual void Kill()
        {
            if(m_singleton != nullptr)
            {
                SingletonsCleaner::RemoveFromList(m_singleton);
                delete m_singleton;
                m_singleton = nullptr;
            }
        }

    protected:

        Singleton(){}
        virtual ~Singleton(){}

        static T *m_singleton;
};

template <typename T> T* Singleton<T>::m_singleton = nullptr;



#endif // SINGLETON_H_INCLUDED
