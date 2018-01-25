#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/core/NotificationListener.h"
#include "ALAGE/core/Asset.h"
#include "ALAGE/gfx/SceneObject.h"
#include "ALAGE/gfx/Light.h"

namespace alag
{

class SceneNode;
class Asset;

class SceneEntity : public NotificationListener, public SceneObject
{
    public:
        SceneEntity();
        virtual ~SceneEntity();

        virtual void Render(sf::RenderTarget *) = 0;
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &) = 0;
        virtual void PrepareShader(sf::Shader *);

        bool CanBeLighted();
        bool Is3D();

        void ActivateLighting();
        void DesactivateLighting();

    protected:
        bool m_canBeLighted;
        bool m_is3D;
        bool m_isLighted;

        std::map<float, Light*> m_nearbyLights;

    private:
        sf::Vector3f m_bounds;
};

}

#endif // SCENEENTITY_H
