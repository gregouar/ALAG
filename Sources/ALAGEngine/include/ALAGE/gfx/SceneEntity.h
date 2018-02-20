#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/core/NotificationListener.h"
#include "ALAGE/core/Asset.h"
#include "ALAGE/gfx/SceneObject.h"
#include "ALAGE/gfx/ShadowCaster.h"
#include "ALAGE/gfx/Light.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

class SceneNode;
class Asset;

class SceneEntity : public ShadowCaster
{
    public:
        SceneEntity();
        virtual ~SceneEntity();

        virtual void Render(sf::RenderTarget *) = 0;
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &) = 0;
        virtual void PrepareShader(sf::Shader *);

        bool CanBeLighted();
        bool UsePBR();

        void EnableLighting();
        void DisableLighting();

        float GetZDepth();
        virtual sf::FloatRect GetScreenBoundingRect(const Mat3x3 &) = 0;

        bool IsStatic();
        bool IsAskingForRenderUpdate();

        void SetZDepth(float);
        void SetStatic(bool);

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        void AskForRenderUpdate(bool isAsking = true); //Used for static geometry

        bool m_canBeLighted;
        bool m_usePBR;
        bool m_isLighted;
        bool m_isStatic;


    private:
        sf::Vector3f m_bounds;
        float m_ZDepth; //Should be used for scene without zBuffer

        bool m_isAskingForRenderUpdate;
};

}

#endif // SCENEENTITY_H
