#include "ALAGE/gfx/IsometricScene.h"
#include "ALAGE/gfx/Sprite3DEntity.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

namespace alag
{

Sprite3DEntity::Sprite3DEntity() : Sprite3DEntity(sf::IntRect (0,0,0,0))
{
    //ctor
}

Sprite3DEntity::Sprite3DEntity(const sf::Vector2i &v) : Sprite3DEntity(sf::IntRect (0,0,v.x, v.y))
{
    //ctor
}


Sprite3DEntity::Sprite3DEntity(const sf::IntRect &r) : SpriteEntity(r)
{
    m_texture = nullptr;
    m_canBeLighted = true;
    m_is3D = true;
}


Sprite3DEntity::~Sprite3DEntity()
{
    //dtor
}

void Sprite3DEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void Sprite3DEntity::Render(sf::RenderTarget *w, const sf::RenderStates &state /*const sf::Transform &t,*/ )
{
    if(m_texture != nullptr && m_texture->IsLoaded())
    {
        sf::RenderStates newState = state;
        newState.transform = sf::Transform::Identity;
        newState.transform.translate(state.transform.transformPoint(0,0));

        w->draw((*this), newState);
    }
}

void Sprite3DEntity::PrepareShader(sf::Shader *shader, float isoToCartZFactor)
{
    if(shader != nullptr
    && m_texture != nullptr
    && m_texture->IsLoaded())
    {
        shader->setUniform("colorMap",*m_texture->GetColorMap());
        shader->setUniform("depthMap",*m_texture->GetDepthMap());
        shader->setUniform("normalMap",*m_texture->GetNormalMap());
        shader->setUniform("height",m_texture->GetHeight()*getScale().y);
    }
}


void Sprite3DEntity::SetTexture(Texture3DAsset *texture)
{
    if(m_texture != texture)
    {
        if(m_texture != nullptr)
            StopListeningTo(m_texture);

        m_texture = texture;

        if(texture != nullptr)
            texture->AskForAllNotifications(this);
    }

    if(m_texture != nullptr && texture->GetTexture() != nullptr)
        sf::Sprite::setTexture(*(texture->GetTexture()));
}

void Sprite3DEntity::Notify(NotificationSender* sender, NotificationType notification)
{
    if(sender == m_texture)
    {
        if(notification == AssetLoadedNotification)
            SetTexture(m_texture);
        else if(notification == NotificationSenderDestroyed)
            m_texture = nullptr;
    }
}

}

