#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/core/NotificationListener.h"
#include "ALAGE/core/Asset.h"

namespace alag
{

class SceneNode;
class Asset;

class SceneEntity : public NotificationListener
{
    public:
        friend class SceneNode;

        SceneEntity();
        virtual ~SceneEntity();

        virtual void Render(sf::RenderTarget *) = 0;
        virtual void Render(sf::RenderTarget *, const sf::Transform &) = 0;

        SceneNode *GetParentNode();

    protected:
        SceneNode* SetParentNode(SceneNode*);

        SceneNode *m_parentNode;

    private:
        sf::Vector3f m_bounds;
};

}

#endif // SCENEENTITY_H
