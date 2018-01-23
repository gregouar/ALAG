#include "ALAGE/gfx/Sprite3DEntity.h"
#include "ALAGE/gfx/SceneNode.h"

namespace alag
{

Sprite3DEntity::Sprite3DEntity() : Sprite3DEntity(sf::IntRect (0,0,0,0))
{
    //ctor
}

Sprite3DEntity::Sprite3DEntity(const sf::Vector2i &v) : Sprite3DEntity(sf::IntRect (0,0,v.x, v.y))
{
    //ctor
}


Sprite3DEntity::Sprite3DEntity(const sf::IntRect &r) : SpriteEntity(r)
{
}


Sprite3DEntity::~Sprite3DEntity()
{
    //dtor
}

void Sprite3DEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void Sprite3DEntity::Render(sf::RenderTarget *w, const sf::Transform &t)
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

void Sprite3DEntity::SetTexture(Texture3DAsset *texture)
{
    m_texture = texture;
    if(texture != nullptr)
        sf::Sprite::setTexture(*texture->GetColorMap(this));
}

void Sprite3DEntity::NotifyLoadedAsset(Asset *asset)
{
    if(asset == m_texture)
        SetTexture(m_texture);
}

}

