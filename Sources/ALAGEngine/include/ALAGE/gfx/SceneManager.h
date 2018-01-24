#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/RectEntity.h"
#include "ALAGE/gfx/SpriteEntity.h"

namespace alag
{

class SceneManager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual bool InitRenderer(int, int) = 0;
        virtual void Update(sf::Time);
        virtual void ComputeRenderQueue();
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*) = 0;
        virtual void CleanAll();

        void AskToComputeRenderQueue();

        SceneNode *GetRootNode();

        RectEntity* CreateRectEntity(sf::Vector2f = sf::Vector2f(0,0));
        SpriteEntity* CreateSpriteEntity(sf::Vector2i);
        SpriteEntity* CreateSpriteEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        void DestroyEntity(const EntityTypeID &);
        void DestroyAllEntities();

        void MoveView(sf::Vector2f);
        sf::Vector2f GetViewCenter();

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

    protected:
        EntityTypeID GenerateEntityID();
        void AddEntity(const EntityTypeID &, SceneEntity*);
        void AddToRenderQueue(SceneNode*);

        sf::View m_view; //Should remove this and create CameraEntity
        SceneNode m_rootNode;

        std::list<SceneEntity*> m_renderQueue;

        sf::RenderTarget *m_last_target;

    private:
        std::map<EntityTypeID, SceneEntity*> m_entities;
        EntityTypeID m_curNewId;

        bool m_needToUpdateRenderQueue;


};

}

#endif // SCENEMANAGER_H
