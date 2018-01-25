#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/RectEntity.h"
#include "ALAGE/gfx/SpriteEntity.h"
#include "ALAGE/gfx/Camera.h"
#include "ALAGE/gfx/Light.h"

namespace alag
{

class SceneManager
{
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual void CleanAll();

        //virtual bool InitRenderer(int, int) = 0;
        virtual void Update(sf::Time);
        virtual void ComputeRenderQueue();
        virtual sf::View GenerateView(Camera*);
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*) = 0;

        void AskToComputeRenderQueue();

        SceneNode *GetRootNode();

        RectEntity*     CreateRectEntity(sf::Vector2f = sf::Vector2f(0,0));
        SpriteEntity*   CreateSpriteEntity(sf::Vector2i);
        SpriteEntity*   CreateSpriteEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        Light* CreateLight(LightType = OmniLight, sf::Vector3f = sf::Vector3f(0,0,-1), sf::Color = sf::Color::White);
        Camera* CreateCamera(sf::Vector2f viewSize);

        void DestroyCreatedObject(const ObjectTypeID &);
        void DestroyAllCreatedObjects();

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        virtual void SetCurrentCamera(Camera *);
        virtual void SetAmbientLight(sf::Color);

    protected:
        ObjectTypeID GenerateObjectID();
        void AddCreatedObject(const ObjectTypeID &, SceneObject*);
        void AddToRenderQueue(SceneNode*);

        virtual int UpdateLighting(std::multimap<float, Light*> &lightList, int = GL_MAX_LIGHTS);

        //sf::View m_view; //Should remove this and create CameraEntity
        Camera *m_currentCamera;
        SceneNode m_rootNode;

        sf::Color m_ambientLight;

        std::list<SceneEntity*> m_renderQueue;
        sf::RenderTarget *m_last_target;

    private:
        std::map<ObjectTypeID, SceneObject*> m_createdObjects;
        ObjectTypeID m_curNewId;

        bool m_needToUpdateRenderQueue;


};

}

#endif // SCENEMANAGER_H
