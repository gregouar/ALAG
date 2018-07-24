
#include <SFML/System.hpp>
#include <sstream>

#include "ALAGE/core/GApp.h"
#include "ALAGE/utils/Profiler.h"
#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Parser.h"

namespace alag
{


const char *GApp::DEFAULT_APP_NAME = "ALAGEngine";
const char *GApp::DEFAULT_CONFIG_FILE = "config.ini";
const char *GApp::DEFAULT_SCREENSHOTPATH = "../screenshots/";

const char *GApp::DEFAULT_WINDOW_WIDTH = "1024";
const char *GApp::DEFAULT_WINDOW_HEIGHT = "768";
const char *GApp::DEFAULT_SRGB = "false";
const char *GApp::DEFAULT_VSYNC = "false";

const bool GApp::ENABLE_PROFILER = false;

GApp::GApp() : GApp(DEFAULT_APP_NAME)
{
}

GApp::GApp(const std::string& name)
{
    m_name = name;
    m_screenshot_nbr = 1;
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
        Logger::Error(error_report);

        videoMode = sf::VideoMode(Parser::ParseInt(DEFAULT_WINDOW_WIDTH),
                                  Parser::ParseInt(DEFAULT_WINDOW_HEIGHT));

        if(!videoMode.isValid())
        {
            std::ostringstream error_report;
            error_report<<"Invalid default resolution "<<videoMode.width<<"x"<<videoMode.height;
            Logger::FatalError(error_report);
            return (false);
        }
    }

    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.antialiasingLevel = 0;

    m_window.create(videoMode, m_name, sf::Style::Close, contextSettings);
    m_window.setVerticalSyncEnabled(Config::GetBool("window","width",DEFAULT_VSYNC));

    return (true);
}

int GApp::Run(GState *state)
{
    m_running = true;

    if(!this->Init())
    {
        Logger::FatalError("Could not initialize application");
        return 1;
    }
    m_stateManager.AttachGApp(this);
    m_stateManager.Switch(state);

    Logger::Write("Starting application");

    return this->Loop();
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
        Profiler::ResetLoop(ENABLE_PROFILER);

        m_eventManager.Update(&m_window);

        m_window.clear();

        if(m_stateManager.Peek() == nullptr)
            this->Stop();
        else {
            m_stateManager.HandleEvents(&m_eventManager);

            Profiler::PushClock("GApp update");
            m_stateManager.Update(elapsed_time);
            Profiler::PopClock();

            Profiler::PushClock("GApp draw");
            m_stateManager.Draw(&m_window);
            Profiler::PopClock();
        }

        Profiler::PushClock("GApp display");
        m_window.display();
        Profiler::PopClock();


        /** NEED TO CHANGE TO CONFIG::PRINTSCREENKEY **/
        if(m_eventManager.KeyPressed(sf::Keyboard::P))
            Printscreen();
    }

    return 0;
}

void GApp::Printscreen()
{
    std::ostringstream buf;
    buf<<DEFAULT_SCREENSHOTPATH<<"screenshot"<<m_screenshot_nbr++<<".png";

    sf::Vector2u windowSize = m_window.getSize();
    sf::Texture texture;
    texture.create(windowSize.x, windowSize.y);
    texture.update(m_window);
    texture.copyToImage().saveToFile(buf.str());
}

sf::Vector2u GApp::GetWindowSize()
{
    return m_window.getSize();
}


}
