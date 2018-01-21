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
        virtual void CleanAll();

        SceneNode *GetRootNode();

        RectEntity* CreateRectEntity(sf::FloatRect = sf::FloatRect(0,0,0,0));

        void DestroyEntity(const EntityTypeID &);
        void DestroyAllEntities();

        EntityTypeID GenerateEntityID();

    protected:
        void AddEntity(const EntityTypeID &, SceneEntity*);

        sf::View m_view;
        SceneNode m_rootNode;

    private:
        std::map<EntityTypeID, SceneEntity*> m_entities;
        EntityTypeID m_curNewId;


};

}

#endif // SCENEMANAGER_H
