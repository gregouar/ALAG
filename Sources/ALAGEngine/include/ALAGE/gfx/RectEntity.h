#ifndef RECTENTITY_H
#define RECTENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/Texture3DAsset.h"
#include "ALAGE/gfx/SceneEntity.h"

namespace alag
{

class RectEntity : public SceneEntity, protected sf::RectangleShape
{
    public:
        RectEntity();
        RectEntity(sf::Vector2f rectSize);
        virtual ~RectEntity();

        virtual void Render(sf::RenderTarget *);
        virtual void Render(sf::RenderTarget *, const sf::Transform &);

        void SetTexture(TextureAsset *t);
        void SetCenter(sf::Vector2f);

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        TextureAsset *m_texture;

    private:
};

}

#endif // RECTENTITY_H
