#include "ALAGE/gfx/RectEntity.h"

#include "ALAGE/gfx/IsometricScene.h"
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

    m_canBeLighted = true;
    m_is3D = false;
}

RectEntity::~RectEntity()
{
    //dtor
}

void RectEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void RectEntity::Render(sf::RenderTarget *w, const sf::RenderStates &state)
{
    if(m_texture != nullptr)
    {
        sf::RenderStates newState = state;
        newState.transform = sf::Transform::Identity;
        newState.transform.translate(state.transform.transformPoint(0,0));

        w->draw((*this), state);
    }
   // w->draw((*this), state);
}


void RectEntity::PrepareShader(sf::Shader *shader)
{
    if(Is3D())
    if(shader != nullptr
    && m_texture != nullptr
    && m_texture->IsLoaded())
    {
        Texture3DAsset *myTexture3D = (Texture3DAsset*) m_texture;
        shader->setUniform("colorMap",*myTexture3D->GetColorMap());
        shader->setUniform("depthMap",*myTexture3D->GetDepthMap());
        shader->setUniform("height",myTexture3D->GetHeight()*getScale().y*DEPTH_BUFFER_NORMALISER);
    }
}



void RectEntity::SetTexture(TextureAsset *texture)
{
    m_is3D = false;

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


void RectEntity::SetTexture(Texture3DAsset *texture)
{
    m_is3D = true;

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
