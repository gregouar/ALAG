#include "ALAGE/gfx/SpriteEntity.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/SceneNode.h"
#include <SFML/Graphics.hpp>

namespace alag
{

SpriteEntity::SpriteEntity() : SpriteEntity(sf::IntRect (0,0,0,0))
{
    //ctor
}

SpriteEntity::SpriteEntity(const sf::Vector2i &v) : SpriteEntity(sf::IntRect (0,0,v.x, v.y))
{
    //ctor
}


SpriteEntity::SpriteEntity(const sf::IntRect &r)
{
    sf::Sprite::setTextureRect(r);
    m_texture = nullptr;
    m_canBeLighted = true;
    m_is3D = false;
}


SpriteEntity::~SpriteEntity()
{
    //dtor
}

void SpriteEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void SpriteEntity::Render(sf::RenderTarget *w, const sf::RenderStates &state)
{
    //w->draw((*this), state);

    if(m_texture != nullptr && m_texture->IsLoaded())
    {
        sf::RenderStates newState = state;
        newState.transform = sf::Transform::Identity;
        newState.transform.translate(state.transform.transformPoint(0,0));

        w->draw((*this), newState);
    }
}


void SpriteEntity::PrepareShader(sf::Shader *shader)
{
    if(shader != nullptr
    && m_texture != nullptr
    && m_texture->IsLoaded())
    {
        shader->setUniform("colorMap",*m_texture->GetTexture());
        if(Is3D())
        {
            Texture3DAsset *myTexture3D = (Texture3DAsset*) m_texture;
            shader->setUniform("depthMap",*myTexture3D->GetDepthMap());
            shader->setUniform("normalMap",*myTexture3D->GetNormalMap());
            shader->setUniform("height",myTexture3D->GetHeight()*getScale().y);
        } else {
            shader->setUniform("depthMap",*AssetHandler<TextureAsset>::Instance()->GetDummyAsset()->GetTexture());
            shader->setUniform("normalMap",*AssetHandler<TextureAsset>::Instance()->GetDummyAsset()->GetTexture());
            shader->setUniform("height",0);
        }
    }

}



void SpriteEntity::SetTexture(TextureAsset *texture)
{
    m_is3D = false;

    if(m_texture != texture)
    {
        if(m_texture != nullptr)
            StopListeningTo(m_texture);

        m_texture = texture;

        if(texture != nullptr)
            texture->AskForAllNotifications(this);
    }

    if(texture != nullptr && texture->GetTexture() != nullptr)
        sf::Sprite::setTexture(*(texture->GetTexture()));
}


void SpriteEntity::SetTexture(Texture3DAsset *texture)
{
    SetTexture((TextureAsset*) texture);
    m_is3D = true;
}



void SpriteEntity::SetCenter(float x, float y)
{
    SetCenter(sf::Vector2f(x,y));
}

void SpriteEntity::SetCenter(sf::Vector2f c)
{
    sf::Sprite::setOrigin(c);
}

sf::Vector2f SpriteEntity::GetCenter()
{
    return sf::Sprite::getOrigin();
}

void SpriteEntity::Notify(NotificationSender* sender, NotificationType notification)
{
    if(sender == m_texture)
    {
        if(notification == AssetLoadedNotification)
        {
            bool was3D = m_is3D;
            SetTexture(m_texture);
            m_is3D = was3D;
        }
        else if(notification == NotificationSenderDestroyed)
            m_texture = nullptr;
    }
}

}
