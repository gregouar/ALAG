#ifndef MAPITERATOR_H
#define MAPITERATOR_H

#include <map>

template<class T1, class T2> class MapIterator
{
    public:
        MapIterator(){}
        MapIterator(typename std::map<T1, T2>::iterator it, typename std::map<T1, T2>::iterator endIt)
        {m_iterator = it, m_end = endIt;}

        virtual ~MapIterator(){}

        const T2 GetElement(){
            if(!IsAtTheEnd()) return m_iterator->second;
            return nullptr;
         }

         const T1 GetKey(){
            if(!IsAtTheEnd()) return m_iterator->first;
            return nullptr;
         }

        bool IsAtTheEnd(){return (m_iterator == m_end);};

        void operator++(int){ m_iterator++;};

    private:
        typename std::map<T1,T2>::iterator m_iterator;
        typename std::map<T1,T2>::iterator m_end;
};

//#include "ALAGE/utils/ListIterator.inc"

#endif // LISTITERATOR_H
