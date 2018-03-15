#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/PBRTextureAsset.h"
#include "ALAGE/gfx/SceneEntity.h"


namespace alag
{

class SpriteEntity : public SceneEntity, protected sf::Sprite
{
    public:
        SpriteEntity();
        SpriteEntity(const sf::Vector2i &spriteSize);
        SpriteEntity(const sf::IntRect &textureRect);
        virtual ~SpriteEntity();

        virtual void Render(sf::RenderTarget *);
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &);

        virtual void PrepareShader(sf::Shader*);

        virtual void ComputeShadow(Light*){}

        void Rotate(float r);

        virtual void SetTexture(TextureAsset *t);
        virtual void SetTexture(PBRTextureAsset *t);
        void SetCenter(float x, float y);
        void SetCenter(sf::Vector2f);
        void SetScale(float x, float y);
        void SetScale(sf::Vector2f);
        void SetRotation(float r);
        void SetColor(sf::Color);



        void SetShadowCastingType(ShadowCastingType);

        sf::Vector2f GetCenter();
        sf::Vector2f GetScale();
        virtual sf::FloatRect GetScreenBoundingRect(const Mat3x3 &);

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        TextureAsset *m_texture;

    private:
};

}

#endif // SPRITEENTITY_H
