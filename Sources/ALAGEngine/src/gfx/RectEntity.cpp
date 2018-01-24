#include "ALAGE/gfx/RectEntity.h"

#include "ALAGE/gfx/SceneNode.h"

namespace alag
{

RectEntity::RectEntity()  : RectEntity(sf::Vector2f (0,0))
{
    //ctor
}

RectEntity::RectEntity(sf::Vector2f s) : sf::RectangleShape(s)
{
    m_texture = nullptr;
}

RectEntity::~RectEntity()
{
    //dtor
}

void RectEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void RectEntity::Render(sf::RenderTarget *w, const sf::Transform &t)
{
    sf::Transform totalTransform;
    totalTransform = sf::Transform::Identity;
    if(GetParentNode() != nullptr)
    {
        sf::Vector3f globalPos = GetParentNode()->GetGlobalPosition();
        totalTransform.translate(globalPos.x, globalPos.y);
    }

    w->draw((*this), t*totalTransform);
}

void RectEntity::SetTexture(TextureAsset *texture)
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
        sf::RectangleShape::setTexture(m_texture->GetTexture());
}

void RectEntity::SetCenter(sf::Vector2f c)
{
    sf::RectangleShape::setOrigin(c);
}

void RectEntity::Notify(NotificationSender* sender, NotificationType notification)
{
    if(m_texture == sender)
    {
        if(notification == AssetLoadedNotification)
            SetTexture(m_texture);
        else if(notification == NotificationSenderDestroyed)
            m_texture = nullptr;
    }
}

}
