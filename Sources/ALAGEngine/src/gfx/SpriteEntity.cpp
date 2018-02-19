#include "ALAGE/gfx/SpriteEntity.h"


#include "ALAGE/gfx/iso/PBRIsoScene.h"
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


SpriteEntity::SpriteEntity(const sf::IntRect &r) : SceneEntity()
{
    sf::Sprite::setTextureRect(r);
    m_texture = nullptr;
    m_canBeLighted = true;
    m_usePBR = false;
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
        //newState.shader = NULL;

         w->draw((*this), newState);

       /*for(int i = 0 ; i < 10 ; ++i){
            for(int j = 0 ; j < 10 ; ++j)
            {
                newState.transform.translate(10,0);
                w->draw((*this), newState);
            }
                newState.transform.translate(-100,10);
        }*/


        /*sf::VertexArray quad(sf::Quads, 4*100);
        for(int i = 0 ; i < 10 ; ++i)
        for(int j = 0 ; j < 10 ; ++j)
        {
            quad[(i+j*10)*4+0].position = sf::Vector2f(i*10,j*10) -sf::Sprite::getOrigin();
            quad[(i+j*10)*4+1].position = sf::Vector2f(i*10,j*10)-sf::Sprite::getOrigin() + sf::Vector2f(sf::Sprite::getTextureRect().width,0);
            quad[(i+j*10)*4+2].position = sf::Vector2f(i*10,j*10)-sf::Sprite::getOrigin() + sf::Vector2f(sf::Sprite::getTextureRect().width,
                                                           sf::Sprite::getTextureRect().height);
            quad[(i+j*10)*4+3].position = sf::Vector2f(i*10,j*10)-sf::Sprite::getOrigin() + sf::Vector2f(0,sf::Sprite::getTextureRect().height);

            quad[(i+j*10)*4+0].texCoords = sf::Vector2f(sf::Sprite::getTextureRect().left,
                                             sf::Sprite::getTextureRect().top);
            quad[(i+j*10)*4+1].texCoords = sf::Vector2f(sf::Sprite::getTextureRect().left + sf::Sprite::getTextureRect().width,
                                             sf::Sprite::getTextureRect().top);
            quad[(i+j*10)*4+2].texCoords = sf::Vector2f(sf::Sprite::getTextureRect().left + sf::Sprite::getTextureRect().width,
                                             sf::Sprite::getTextureRect().top + sf::Sprite::getTextureRect().height);
            quad[(i+j*10)*4+3].texCoords = sf::Vector2f(sf::Sprite::getTextureRect().left,
                                             sf::Sprite::getTextureRect().top + sf::Sprite::getTextureRect().height);
        }

        newState.texture = sf::Sprite::getTexture();

        w->draw(quad, newState);*/
    }
}


void SpriteEntity::PrepareShader(sf::Shader *shader)
{
    if(shader != nullptr
    && m_texture != nullptr
    && m_texture->IsLoaded())
    {
        if(UsePBR())
        {
            PBRTextureAsset *myPBRTexture = (PBRTextureAsset*) m_texture;
            myPBRTexture->PrepareShader(shader);
            shader->setUniform("p_height",myPBRTexture->GetHeight()*GetScale().y*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
        } else {
            shader->setUniform("map_albedo",*m_texture->GetTexture());
            shader->setUniform("enable_normalMap", false);
            shader->setUniform("enable_depthMap", false);
            shader->setUniform("p_height",0);
        }
    }
}



void SpriteEntity::SetTexture(TextureAsset *texture)
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

    if(texture != nullptr && texture->GetTexture() != nullptr)
        sf::Sprite::setTexture(*(texture->GetTexture()));
}


void SpriteEntity::SetTexture(PBRTextureAsset *texture)
{
    SetTexture((TextureAsset*) texture);
    m_usePBR = true;
}



void SpriteEntity::SetCenter(float x, float y)
{
    SetCenter(sf::Vector2f(x,y));
}

void SpriteEntity::SetCenter(sf::Vector2f c)
{
    sf::Sprite::setOrigin(c);
}

void SpriteEntity::SetScale(float x, float y)
{
    SetScale(sf::Vector2f(x,y));
}

void SpriteEntity::SetScale(sf::Vector2f c)
{
    sf::Sprite::setScale(c);
}

void SpriteEntity::SetColor(sf::Color c)
{
    sf::Sprite::setColor(c);
}


void SpriteEntity::SetShadowCastingType(ShadowCastingType type)
{
    ShadowCaster::ForceShadowCastingType(type);
}

sf::Vector2f SpriteEntity::GetCenter()
{
    return sf::Sprite::getOrigin();
}

sf::Vector2f SpriteEntity::GetScale()
{
    return sf::Sprite::getScale();
}


void SpriteEntity::Notify(NotificationSender* sender, NotificationType notification)
{
    if(sender == m_texture)
    {
        if(notification == AssetLoadedNotification)
        {
            bool wasPBR = m_usePBR;
            SetTexture(m_texture);
            m_usePBR = wasPBR;
        }
        else if(notification == NotificationSenderDestroyed)
            m_texture = nullptr;

        ShadowCaster::AskForAllShadowUpdate();
    }
}

}
