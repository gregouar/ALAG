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


#include "GState.h"

namespace alag
{

GState::GState()
{
    //ctor
}

GState::~GState()
{
    //dtor
}

void GState::SetManager(StateManager *manager)
{
    m_manager = manager;
}

void GState::Pause()
{
    m_running = false;
}

void GState::Resume()
{
    m_running = true;
}

}
