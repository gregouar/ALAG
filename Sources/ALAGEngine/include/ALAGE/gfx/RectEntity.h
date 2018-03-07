#ifndef RECTENTITY_H
#define RECTENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/PBRTextureAsset.h"
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
        virtual void Render(sf::RenderTarget *, const sf::RenderStates &);
        virtual void PrepareShader(sf::Shader *);

        virtual void ComputeShadow(Light*){}

        void SetTexture(TextureAsset *t);
        void SetTexture(PBRTextureAsset *t);
        void SetTextureRect(const sf::IntRect &);
        void SetCenter(sf::Vector2f);
        void SetColor(sf::Color);
        void SetHeightFactor(float);


        sf::Vector2f GetCenter();
        virtual sf::FloatRect GetScreenBoundingRect(const Mat3x3 &);

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        TextureAsset *m_texture;
        float         m_heightFactor;

    private:
        bool m_customTextureRect;
};

}

#endif // RECTENTITY_H
