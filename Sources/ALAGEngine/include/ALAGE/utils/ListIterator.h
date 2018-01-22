#ifndef LISTITERATOR_H
#define LISTITERATOR_H

#include <list>

template<class T> class ListIterator
{
    public:
        ListIterator(){}
        ListIterator(typename std::list<T>::iterator it, typename std::list<T>::iterator endIt)
        {m_iterator = it, m_end = endIt;}

        virtual ~ListIterator(){}

        const T GetElement(){
            if(!IsAtTheEnd()) return *m_iterator;
            return nullptr;
         }

        bool IsAtTheEnd(){return (m_iterator == m_end);};

        void operator++(int){ m_iterator++;};

    private:
        typename std::list<T>::iterator m_iterator;
        typename std::list<T>::iterator m_end;
};

//#include "ALAGE/utils/ListIterator.inc"

#endif // LISTITERATOR_H
