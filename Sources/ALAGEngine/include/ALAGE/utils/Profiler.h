#ifndef PROFILER_H
#define PROFILER_H

#include <stack>
#include <sstream>
#include <SFML/System.hpp>
#include "ALAGE/utils/Singleton.h"

struct ProfilerClock
{
    sf::Clock   clock;
    std::string text;
};

struct ProfilerTime
{
    sf::Time    time;
    std::string text;
};

class Profiler : public Singleton<Profiler>
{
    public:
        friend class Singleton<Profiler>;

        static void ResetLoop(bool print);
        static void PushClock(const std::string&);
        static void PopClock();

    protected:
        Profiler();
        virtual ~Profiler();

        sf::Mutex m_profilerMutex;

    private:
        sf::Clock m_loopClock;
        std::stack<ProfilerClock> m_clocks;
        std::list<ProfilerTime> m_times;

};

#endif // PROFILER_H
