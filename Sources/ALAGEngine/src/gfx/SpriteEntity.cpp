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
    m_texture = texture;
    if(texture != nullptr)
        sf::Sprite::setTexture(*texture->GetTexture(this));
}

void SpriteEntity::SetCenter(float x, float y)
{
    SetCenter(sf::Vector2f(x,y));
}

void SpriteEntity::SetCenter(sf::Vector2f c)
{
    sf::Sprite::setOrigin(c);
}

void SpriteEntity::NotifyLoadedAsset(Asset *asset)
{
    if(asset == m_texture)
        SetTexture(m_texture);
}

}
