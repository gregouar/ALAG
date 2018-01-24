#include "ALAGE/gfx/SpriteEntity.h"

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
    w->draw((*this), state);
}

void SpriteEntity::SetTexture(TextureAsset *texture)
{
    if(m_texture != texture)
    {
        if(m_texture != nullptr)
            m_texture->RemoveFromAllNotificationList(this);

        m_texture = texture;

        if(texture != nullptr)
            texture->AskForAllNotifications(this);
    }

    if(m_texture != nullptr)
        sf::Sprite::setTexture(*(texture->GetTexture()));
}

void SpriteEntity::SetCenter(float x, float y)
{
    SetCenter(sf::Vector2f(x,y));
}

void SpriteEntity::SetCenter(sf::Vector2f c)
{
    sf::Sprite::setOrigin(c);
}


void SpriteEntity::Notify(NotificationSender* sender, NotificationType notification)
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
