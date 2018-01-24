#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SFML/Window.hpp>

namespace alag{

class EventManager
{
    public:
        EventManager();
        virtual ~EventManager();

        void Update(sf::Window *);

        bool KeyPressed(sf::Keyboard::Key);
        bool KeyIsPressed(sf::Keyboard::Key);
        bool KeyReleased(sf::Keyboard::Key);

        bool MouseButtonPressed(sf::Mouse::Button);
        bool MouseButtonIsPressed(sf::Mouse::Button);
        bool MouseButtonReleased(sf::Mouse::Button);

        sf::Vector2i MousePosition();

        bool IsAskingToClose();

    protected:

    private:
        bool m_keyPressed[sf::Keyboard::KeyCount];
        bool m_keyIsPressed[sf::Keyboard::KeyCount];
        bool m_keyReleased[sf::Keyboard::KeyCount];

        bool m_mouseButtonPressed[sf::Mouse::ButtonCount];
        bool m_mouseButtonIsPressed[sf::Mouse::ButtonCount];
        bool m_mouseButtonReleased[sf::Mouse::ButtonCount];

        sf::Vector2i m_mousePosition;

        bool m_askingToClose;
};

}

#endif // EVENTMANAGER_H
