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
}


SpriteEntity::~SpriteEntity()
{
    //dtor
}

void SpriteEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void SpriteEntity::Render(sf::RenderTarget *w, const sf::Transform &t)
{
    sf::Transform totalTransform;
    totalTransform = sf::Transform::Identity;
    if(GetParentNode() != nullptr)
    {
        sf::Vector3f globalPos = GetParentNode()->GetGlobalPosition();
        totalTransform.translate(t.transformPoint(globalPos.x, globalPos.y));
    }

    w->draw((*this), totalTransform);
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
