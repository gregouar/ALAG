#include "ALAGE/gfx/RectEntity.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/utils/Mathematics.h"

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
    m_usePBR = false;

    m_customTextureRect = false;
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
        w->draw((*this), state);
    AskForRenderUpdate(false);
}


void RectEntity::PrepareShader(sf::Shader *shader)
{
    if(shader != nullptr
    && m_texture != nullptr)
    {
    //&& m_texture->IsLoaded())

        if(UsePBR())
        {
            PBRTextureAsset *myPBRTexture = (PBRTextureAsset*) m_texture;
            myPBRTexture->PrepareShader(shader);

        } else {
            shader->setUniform("map_albedo",*m_texture->GetTexture());
            shader->setUniform("p_height",0);
            shader->setUniform("enable_depthMap", false);
            shader->setUniform("enable_normalMap", false);
        }
    }
}



void RectEntity::SetTexture(TextureAsset *texture)
{
    m_usePBR = false;

    if(m_texture != texture)
    {
        if(m_texture != nullptr)
            StopListeningTo(m_texture);

        m_texture = texture;

        if(texture != nullptr)
            texture->AskForAllNotifications(this);
    }

    if(m_texture != nullptr && m_texture->GetTexture() != nullptr)
    {
        sf::IntRect prevRect = sf::RectangleShape::getTextureRect();
        sf::RectangleShape::setTexture(m_texture->GetTexture(),true);
        if(m_customTextureRect)
            SetTextureRect(prevRect);
    }
}


void RectEntity::SetTexture(PBRTextureAsset *texture)
{
    SetTexture((TextureAsset*) texture);
    m_usePBR = true;
}

void RectEntity::SetTextureRect(const sf::IntRect &rect)
{
    sf::RectangleShape::setTextureRect(rect);
    m_customTextureRect = true;
}

void RectEntity::SetCenter(sf::Vector2f c)
{
    sf::RectangleShape::setOrigin(c);
}

void RectEntity::SetColor(sf::Color c)
{
    sf::RectangleShape::setFillColor(c);
}

sf::FloatRect RectEntity::GetScreenBoundingRect(const Mat3x3& transMat)
{
    sf::FloatRect b(0,0,sf::RectangleShape::getSize().x,sf::RectangleShape::getSize().y);
   /* b.position = -GetCenter();
    b.size = -sf::RectangleShape::getSize();*/

    /** NEED TO COMPUTE RECT IN SCREEN POS **/

    return b;
}

sf::Vector2f RectEntity::GetCenter()
{
    return sf::RectangleShape::getOrigin();
}

void RectEntity::Notify(NotificationSender* sender, NotificationType notification)
{
    if(m_texture == sender)
    {
        if(notification == Notification_AssetLoaded)
        {
            bool wasPBR = m_usePBR;
            SetTexture(m_texture);
            m_usePBR = wasPBR;
        }
        else if(notification == Notification_SenderDestroyed)
            m_texture = nullptr;

        AskForRenderUpdate();
    }

    SceneEntity::Notify(sender, notification);
}


}
