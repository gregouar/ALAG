#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#include <iostream>

template<class T> class Singleton
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

template <class T> T Singleton<T>::m_singleton=T();


#endif // SINGLETON_H_INCLUDED
