#ifndef SPRITE3DENTITY_H
#define SPRITE3DENTITY_H

#include "ALAGE/gfx/SpriteEntity.h"

namespace alag
{

class Sprite3DEntity : public SpriteEntity
{
    public:
        Sprite3DEntity();
        Sprite3DEntity(const sf::Vector2i &spriteSize);
        Sprite3DEntity(const sf::IntRect &textureRect);
        virtual ~Sprite3DEntity();

        virtual void Render(sf::RenderTarget *);
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &);
        virtual void PrepareShader(sf::Shader *, float isoToCartZFactor);

        virtual void SetTexture(Texture3DAsset *t);

        virtual void Notify(NotificationSender* , NotificationType);


    protected:
        Texture3DAsset *m_texture;

    private:
};

}

#endif // SPRITE3DENTITY_H
