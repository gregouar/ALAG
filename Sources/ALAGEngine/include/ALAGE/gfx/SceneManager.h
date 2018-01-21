#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/RectEntity.h"

namespace alag
{

class SceneManager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual bool InitRenderer() = 0;
        virtual void RenderScene(sf::RenderTarget*) = 0;

        SceneNode *GetRootNode();

        RectEntity* CreateRectEntity(sf::FloatRect = sf::FloatRect(0,0,0,0));

    protected:
        sf::View m_view;
        SceneNode m_rootNode;

    private:

};

}

#endif // SCENEMANAGER_H
