#ifndef SINGLETONSCLEANER_H
#define SINGLETONSCLEANER_H

#include <list>

class KillableSingleton
{
    friend class SingletonsCleaner;

    private:
        virtual void Kill() = 0;
};

class SingletonsCleaner
{
    public:
       // friend  void KillableSingleton::Kill();

        static SingletonsCleaner* Instance(void);
        static void AddToList(KillableSingleton* singleton);
        static void CleanAll();
        static void RemoveFromList(KillableSingleton* singleton);

    protected:

        SingletonsCleaner(){}
        virtual ~SingletonsCleaner(){CleanAll();}

        std::list<KillableSingleton*> m_singletonsList;

        static SingletonsCleaner m_instance;
};


#endif // SINGLETONSCLEANER_H
