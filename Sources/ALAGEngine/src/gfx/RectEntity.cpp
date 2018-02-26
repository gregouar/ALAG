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

    sf::Vector2f upper_left(0,0), lower_right(0,0);

    for(size_t i = 0 ; i < 3 ; ++i)
    {
        sf::Vector2f p(0,0);

        if(i == 0)
            p = sf::Vector2f(b.width,0);
        else if(i == 1)
            p = sf::Vector2f(b.width, b.height);
        else if(i == 2)
            p = sf::Vector2f(0, b.height);

        p = transMat * p;

        if(p.x < upper_left.x)
            upper_left.x = p.x;
        if(p.x > lower_right.x)
            lower_right.x = p.x;
        if(p.y < upper_left.y)
            upper_left.y = p.y;
        if(p.y > lower_right.y)
            lower_right.y = p.y;
    }

    sf::Vector2f c = transMat * sf::RectangleShape::getOrigin();

    b.left = upper_left.x - c.x;
    b.top = upper_left.y - c.y;
    b.width  = lower_right.x - upper_left.x;
    b.height = lower_right.y - upper_left.y;

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
