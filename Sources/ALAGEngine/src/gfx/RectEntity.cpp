#include "ALAGE/gfx/RectEntity.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/IsometricScene.h"
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
    m_is3D = false;

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
}


void RectEntity::PrepareShader(sf::Shader *shader)
{
    if(shader != nullptr
    && m_texture != nullptr)
    {
    //&& m_texture->IsLoaded())
        shader->setUniform("colorMap",*m_texture->GetTexture());
        if(Is3D())
        {
            Texture3DAsset *myTexture3D = (Texture3DAsset*) m_texture;
            shader->setUniform("depthMap",*myTexture3D->GetDepthMap());
            shader->setUniform("normalMap",*myTexture3D->GetNormalMap());
            shader->setUniform("height",myTexture3D->GetHeight());
        } else {
            shader->setUniform("depthMap",*AssetHandler<TextureAsset>::Instance()->GetDummyAsset()->GetTexture());
            shader->setUniform("normalMap",*AssetHandler<TextureAsset>::Instance()->GetDummyAsset()->GetTexture());
            shader->setUniform("height",0);
        }
        shader->setUniform("normalProjMat",sf::Glsl::Mat3(IdMat3X3));
        shader->setUniform("isoToCartZFactor",0);
    }
}



void RectEntity::SetTexture(TextureAsset *texture)
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

    if(m_texture != nullptr && m_texture->GetTexture() != nullptr)
    {
        sf::IntRect prevRect = sf::RectangleShape::getTextureRect();
        sf::RectangleShape::setTexture(m_texture->GetTexture(),true);
        if(m_customTextureRect)
            SetTextureRect(prevRect);
    }
}


void RectEntity::SetTexture(Texture3DAsset *texture)
{
    SetTexture((TextureAsset*) texture);
    m_is3D = true;
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

sf::Vector2f RectEntity::GetCenter()
{
    return sf::RectangleShape::getOrigin();
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
