#include "ALAGE/core/EventManager.h"

namespace alag{

EventManager::EventManager()
{
    for(size_t i = 0 ; i < sf::Keyboard::KeyCount ; i++)
    {
        m_keyPressed[i] = false;
        m_keyReleased[i] = false;
    }

    m_mousePosition = sf::Vector2u(0,0);
    m_askingToClose = false;
}

EventManager::~EventManager()
{
    //dtor
}


void EventManager::Update(sf::Window *window)
{
    sf::Event event;

    for(size_t i = 0 ; i < sf::Keyboard::KeyCount ; i++)
        m_keyReleased[i] = false;

    while (window->pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                m_askingToClose = true;
                break;

            case sf::Event::KeyPressed:
                m_keyPressed[event.key.code] = true;
                break;

            case sf::Event::KeyReleased:
                m_keyPressed[event.key.code] = false;
                m_keyReleased[event.key.code] = true;
                break;

            case sf::Event::MouseMoved:
                m_mousePosition.x = event.mouseMove.x;
                m_mousePosition.y = event.mouseMove.y;
                break;

            default:
                break;
        }
    }
}

bool EventManager::KeyPressed(sf::Keyboard::Key key)
{
    return m_keyPressed[key];
}

bool EventManager::KeyReleased(sf::Keyboard::Key key)
{
    return m_keyReleased[key];
}

sf::Vector2u EventManager::MousePosition()
{
    return m_mousePosition;
}

bool EventManager::IsAskingToClose()
{
    return m_askingToClose;
}


}
