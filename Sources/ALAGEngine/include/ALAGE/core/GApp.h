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


#ifndef GAPP_H
#define GAPP_H

#include <SFML/Graphics.hpp>
#include "ALAGE/core/Config.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"

namespace alag
{


class GApp
{
    public:
        GApp();
        GApp(const std::string&);
        virtual ~GApp();

        bool Init();

        int Run(GState *);
        void Stop();

        int Loop();

    protected:

    private:
        bool m_running;
        std::string m_name;

        sf::RenderWindow m_window;

       // alag::Config m_config;
        alag::StateManager m_stateManager;
        alag::EventManager m_eventManager;

    public:
        static const char *DEFAULT_APP_NAME;
        static const char *DEFAULT_CONFIG_FILE;

        static const char *DEFAULT_WINDOW_WIDTH;
        static const char *DEFAULT_WINDOW_HEIGHT;
        static const char *DEFAULT_SRGB;
};

}

#endif // GAPP_H
