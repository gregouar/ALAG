#ifndef DEFAULTSCENE_H
#define DEFAULTSCENE_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/RectEntity.h"
#include "ALAGE/gfx/SpriteEntity.h"
#include "ALAGE/gfx/Camera.h"
#include "ALAGE/gfx/Light.h"

namespace alag
{

class DefaultScene
{
    public:
        DefaultScene();
        virtual ~DefaultScene();

        virtual void CleanAll();

        virtual bool InitRenderer(sf::Vector2u);
        virtual void Update(sf::Time);
        virtual void ComputeRenderQueue();
        virtual sf::View GenerateView(Camera*);
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*) = 0;
        virtual void RenderShadows(std::multimap<float, Light*> &,const sf::View &,
                                   /*const sf::Vector2u &,*/ int = GL_MAX_LIGHTS);

        void AskToComputeRenderQueue();

        SceneNode *GetRootNode();

        RectEntity*     CreateRectEntity(sf::Vector2f = sf::Vector2f(0,0));
        SpriteEntity*   CreateSpriteEntity(sf::Vector2i);
        SpriteEntity*   CreateSpriteEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        Light* CreateLight(LightType = OmniLight, sf::Vector3f = sf::Vector3f(0,0,-1),
                           sf::Color = sf::Color::White);
        Camera* CreateCamera(sf::Vector2f viewSize);

        void DestroyCreatedObject(const ObjectTypeID &);
        void DestroyAllCreatedObjects();

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        virtual void SetCurrentCamera(Camera *);
        virtual void SetAmbientLight(sf::Color);

        //virtual void EnableShadowCasting(ShadowCastingType);
        //virtual void DisableShadowCasting(ShadowCastingType);
        virtual void SetShadowCasting(ShadowCastingType);
        virtual void EnableGammaCorrection();
        virtual void DisableGammaCorrection();

    protected:
        ObjectTypeID GenerateObjectID();
        void AddCreatedObject(const ObjectTypeID &, SceneObject*);
        void AddToRenderQueue(SceneNode*);

        virtual int UpdateLighting(std::multimap<float, Light*> &lightList, int = GL_MAX_LIGHTS);

        //sf::View m_view; //Should remove this and create CameraEntity
        Camera *m_currentCamera;
        SceneNode m_rootNode;

        sf::Color m_ambientLight;
        ShadowCastingType m_shadowCastingOption;
        bool m_enableSRGB;

        std::list<SceneEntity*> m_renderQueue;
        sf::RenderTarget *m_last_target;

    private:
        std::map<ObjectTypeID, SceneObject*> m_createdObjects;
        ObjectTypeID m_curNewId;

        bool m_needToUpdateRenderQueue;


};

}

#endif // DEFAULTSCENE_H
