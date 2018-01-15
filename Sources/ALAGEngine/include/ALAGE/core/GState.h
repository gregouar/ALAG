/*Copyright (C) 2017 Naisse Grégoire

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/


#ifndef GSTATE_H
#define GSTATE_H

#include <SFML/System.hpp>
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
        virtual void Draw() = 0;

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
