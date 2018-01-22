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

        sf::Vector2u MousePosition();

        bool IsAskingToClose();

    protected:

    private:
        bool m_keyPressed[sf::Keyboard::KeyCount];
        bool m_keyIsPressed[sf::Keyboard::KeyCount];
        bool m_keyReleased[sf::Keyboard::KeyCount];

        sf::Vector2u m_mousePosition;

        bool m_askingToClose;
};

}

#endif // EVENTMANAGER_H
