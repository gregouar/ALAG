#include "ALAGE/gfx/RectEntity.h"

namespace alag
{

RectEntity::RectEntity()  : RectEntity(sf::FloatRect (0,0,0,0))
{
    //ctor
}

RectEntity::RectEntity(sf::FloatRect r) : sf::FloatRect(r)
{
    m_texture = nullptr;
}

RectEntity::~RectEntity()
{
    //dtor
}

void RectEntity::SetTexture(TextureAsset *texture)
{
    m_texture = texture;
}

}
