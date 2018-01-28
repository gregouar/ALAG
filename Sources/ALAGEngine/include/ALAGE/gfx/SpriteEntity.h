#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/Texture3DAsset.h"
#include "ALAGE/gfx/SceneEntity.h"


namespace alag
{

class SpriteEntity : public SceneEntity, public sf::Sprite
{
    public:
        SpriteEntity();
        SpriteEntity(const sf::Vector2i &spriteSize);
        SpriteEntity(const sf::IntRect &textureRect);
        virtual ~SpriteEntity();

        virtual void Render(sf::RenderTarget *);
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &);

        virtual void PrepareShader(sf::Shader*);

        virtual void SetTexture(TextureAsset *t);
        virtual void SetTexture(Texture3DAsset *t);
        void SetCenter(float x, float y);
        void SetCenter(sf::Vector2f);

        sf::Vector2f GetCenter();

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        TextureAsset *m_texture;

    private:
};

}

#endif // SPRITEENTITY_H
