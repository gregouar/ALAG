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

        void Printscreen();

        sf::Vector2u GetWindowSize();

    protected:

    private:
        bool m_running;
        std::string m_name;

        sf::RenderWindow m_window;

       // alag::Config m_config;
        alag::StateManager m_stateManager;
        alag::EventManager m_eventManager;

        unsigned int m_screenshot_nbr;

    public:
        static const char *DEFAULT_APP_NAME;
        static const char *DEFAULT_CONFIG_FILE;
        static const char *DEFAULT_SCREENSHOTPATH;

        static const char *DEFAULT_WINDOW_WIDTH;
        static const char *DEFAULT_WINDOW_HEIGHT;
        static const char *DEFAULT_VSYNC;
        static const char *DEFAULT_SRGB;

        static const bool ENABLE_PROFILER;
};

}

#endif // GAPP_H
