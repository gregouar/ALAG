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

#include <SFML/System.hpp>

#include "ALAGE/core/GApp.h"
#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Parser.h"
#include "ALAGE/gfx/GfxEngine.h"

namespace alag
{


const char *GApp::DEFAULT_APP_NAME = "ALAGEngine";
const char *GApp::DEFAULT_CONFIG_FILE = "config.ini";

const char *GApp::DEFAULT_WINDOW_WIDTH = "800";
const char *GApp::DEFAULT_WINDOW_HEIGHT = "600";
const char *GApp::DEFAULT_SRGB = "false";



GApp::GApp()
{
    m_name = DEFAULT_APP_NAME;
}

GApp::GApp(const std::string& name)
{
    m_name = name;
}

GApp::~GApp()
{
}

bool GApp::Init()
{
    Config::Instance()->Load(DEFAULT_CONFIG_FILE);

    Logger::Write("Initializing application");

    sf::VideoMode videoMode = sf::VideoMode(Config::GetInt("window","width",DEFAULT_WINDOW_WIDTH),
                                            Config::GetInt("window","height",DEFAULT_WINDOW_HEIGHT));

    if(!videoMode.isValid())
    {
        std::ostringstream error_report;
        error_report<<"Invalid resolution "<<videoMode.width<<"x"<<videoMode.height;
        Logger::Error(error_report.str());

        videoMode = sf::VideoMode(Parser::ParseInt(DEFAULT_WINDOW_WIDTH),
                                  Parser::ParseInt(DEFAULT_WINDOW_HEIGHT));

        if(!videoMode.isValid())
        {
            std::ostringstream error_report;
            error_report<<"Invalid default resolution "<<videoMode.width<<"x"<<videoMode.height;
            Logger::FatalError(error_report.str());
            return (false);
        }
    }

    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.sRgbCapable = Config::GetBool("window","srgb",DEFAULT_SRGB);

    m_window.create(videoMode, m_name, sf::Style::Close, contextSettings);

    //GfxEngine::Instance()->InitRenderer();

    return (true);
}

int GApp::Run(GState *state)
{
    m_running = true;

    if(!Init())
    {
        Logger::FatalError("Could not initialize application");
        return 1;
    }

    m_stateManager.Switch(state);

    Logger::Write("Starting application");

    return Loop();
}

void GApp::Stop()
{
    Logger::Write("Stopping application");

    m_window.close();
    m_running = false;
}

int GApp::Loop()
{
    sf::Clock clock;

    clock.restart();
    while(m_running)
    {
        sf::Time elapsed_time = clock.restart();

        m_eventManager.Update(&m_window);

        if(m_stateManager.Peek() == NULL)
            Stop();
        else {
            m_stateManager.HandleEvents(&m_eventManager);
            m_stateManager.Update(elapsed_time);
            m_stateManager.Draw(&m_window);
        }

        //GfxEngine::Instance()->RenderWorld(&m_window);
        m_window.display();
    }

    return 0;
}


}
