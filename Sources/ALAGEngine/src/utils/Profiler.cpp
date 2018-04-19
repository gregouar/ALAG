#include "ALAGE/utils/Profiler.h"

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
}

void Profiler::ResetLoop(bool print)
{
    sf::Lock lock(Profiler::Instance()->m_profilerMutex);

    sf::Time total_time = Profiler::Instance()->m_loopClock.restart();

    if(print)
    {
        std::ostringstream buf;

        std::list<ProfilerTime>::iterator timesIt;
        for(timesIt = Profiler::Instance()->m_times.begin() ;
            timesIt != Profiler::Instance()->m_times.end() ; ++timesIt)
        {
            buf<<timesIt->text<<": "
               <<timesIt->time.asMicroseconds()*100.0/total_time.asMicroseconds()<<"%"
               <<std::endl;
        }

        buf<<"Total loop time: "<<total_time.asMilliseconds()<<" ms"<<std::endl;
        std::cout<<buf.str();
    }

    Profiler::Instance()->m_times.clear();
}

void Profiler::PushClock(const std::string &text)
{
    ProfilerClock clock;
    clock.text = text;
    clock.clock.restart();
    Profiler::Instance()->m_clocks.push(clock);
}

void Profiler::PopClock()
{
    if(!Profiler::Instance()->m_clocks.empty())
    {
        ProfilerTime time;
        time.time = Profiler::Instance()->m_clocks.top().clock.getElapsedTime();
        time.text = Profiler::Instance()->m_clocks.top().text;
        Profiler::Instance()->m_clocks.pop();
        Profiler::Instance()->m_times.push_back(time);
    }
}


