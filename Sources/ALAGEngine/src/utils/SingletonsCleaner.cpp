#include "ALAGE/utils/SingletonsCleaner.h"


SingletonsCleaner SingletonsCleaner::m_instance=SingletonsCleaner();

SingletonsCleaner* SingletonsCleaner::Instance(void)
{
    return &m_instance;
}

void SingletonsCleaner::AddToList(KillableSingleton* singleton)
{
    Instance()->m_singletonsList.push_back(singleton);
    Instance()->m_singletonsList.unique();
}

void SingletonsCleaner::CleanAll()
{
    while(!Instance()->m_singletonsList.empty())
        Instance()->m_singletonsList.back()->Kill();
}

void SingletonsCleaner::RemoveFromList(KillableSingleton* singleton)
{
    std::list<KillableSingleton*>::iterator singletonIterator;
    singletonIterator = Instance()->m_singletonsList.begin();
    while(singletonIterator != Instance()->m_singletonsList.end())
    {
        if(*singletonIterator == singleton)
        {
            Instance()->m_singletonsList.erase(singletonIterator);
            singletonIterator = Instance()->m_singletonsList.end();
        } else
            ++singletonIterator;
    }
}


