#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <SFML/Graphics.hpp>

namespace alag
{

class SceneManager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual bool InitRenderer() = 0;
        virtual void RenderScene(sf::RenderTarget*) = 0;

    protected:
        sf::View m_view;

    private:
};

}

#endif // SCENEMANAGER_H
