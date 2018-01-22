#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/Texture3DAsset.h"
#include "ALAGE/gfx/SceneEntity.h"


namespace alag
{

class SpriteEntity : public SceneEntity, private sf::Sprite
{
    public:
        SpriteEntity();
        SpriteEntity(const sf::Vector2i &spriteSize);
        SpriteEntity(const sf::IntRect &textureRect);
        virtual ~SpriteEntity();

        virtual void Render(sf::RenderTarget *);
        virtual void Render(sf::RenderTarget *, const sf::Transform &);

        void SetTexture(TextureAsset *t);
        void SetCenter(sf::Vector2f);

        virtual void NotifyLoadedAsset(Asset*);

    protected:
        TextureAsset *m_texture;

    private:
};

}

#endif // SPRITEENTITY_H
