#include "ALAGE/core/EventManager.h"

namespace alag{

EventManager::EventManager()
{
    for(size_t i = 0 ; i < sf::Keyboard::KeyCount ; ++i)
    {
        m_keyIsPressed[i] = false;
        m_keyPressed[i] = false;
        m_keyReleased[i] = false;
    }

    for(size_t i = 0 ; i < sf::Mouse::ButtonCount ; ++i)
    {
        m_mouseButtonPressed[i] = false;
        m_mouseButtonIsPressed[i] = false;
        m_mouseButtonReleased[i] = false;
    }

    m_mousePosition = sf::Vector2i(0,0);
    m_askingToClose = false;
}

EventManager::~EventManager()
{
    //dtor
}


void EventManager::Update(sf::Window *window)
{
    sf::Event event;

    for(size_t i = 0 ; i < sf::Keyboard::KeyCount ; ++i)
        m_keyReleased[i] = false, m_keyPressed[i] = false;

    for(size_t i = 0 ; i < sf::Mouse::ButtonCount ; ++i)
        m_mouseButtonPressed[i] = false,m_mouseButtonReleased[i] = false;

    while (window->pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                m_askingToClose = true;
                break;

            case sf::Event::KeyPressed:
                m_keyPressed[event.key.code] = true;
                m_keyIsPressed[event.key.code] = true;
                break;

            case sf::Event::KeyReleased:
                m_keyIsPressed[event.key.code] = false;
                m_keyReleased[event.key.code] = true;
                break;

            case sf::Event::MouseButtonPressed:
                m_mouseButtonPressed[event.mouseButton.button] = true;
                m_mouseButtonIsPressed[event.mouseButton.button] = true;
                break;

            case sf::Event::MouseButtonReleased:
                m_mouseButtonIsPressed[event.mouseButton.button] = false;
                m_mouseButtonReleased[event.mouseButton.button] = true;
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

bool EventManager::KeyIsPressed(sf::Keyboard::Key key)
{
    return m_keyIsPressed[key];
}

bool EventManager::KeyReleased(sf::Keyboard::Key key)
{
    return m_keyReleased[key];
}

bool EventManager::MouseButtonPressed(sf::Mouse::Button button)
{
    return m_mouseButtonPressed[button];
}

bool EventManager::MouseButtonIsPressed(sf::Mouse::Button button)
{
    return m_mouseButtonIsPressed[button];
}

bool EventManager::MouseButtonReleased(sf::Mouse::Button button)
{
    return m_mouseButtonReleased[button];
}

sf::Vector2i EventManager::MousePosition()
{
    return m_mousePosition;
}

bool EventManager::IsAskingToClose()
{
    return m_askingToClose;
}


}
