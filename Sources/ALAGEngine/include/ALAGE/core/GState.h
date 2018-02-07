#ifndef GSTATE_H
#define GSTATE_H

#include <SFML/Graphics.hpp>
#include "ALAGE/utils/singleton.h"

namespace alag
{

class StateManager;
class EventManager;

class GState
{
    public:
        GState();
        virtual ~GState();

        virtual void Entered() = 0;
        virtual void Leaving() = 0;
        virtual void Revealed() = 0;
        virtual void Obscuring() = 0;

        virtual void HandleEvents(alag::EventManager *) = 0;
        virtual void Update(sf::Time) = 0;
        virtual void Draw(sf::RenderTarget*) = 0;

        void SetManager(StateManager *);

        void Pause();
        void Resume();

    protected:
        StateManager *m_manager;

    private:
        bool m_running;
};

}

#endif // GSTATE_H
