#include "ALAGE/gfx/iso/IsoRectEntity.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

IsoRectEntity::IsoRectEntity()  : IsoRectEntity(sf::Vector2f (0,0))
{
    //ctor
}

IsoRectEntity::IsoRectEntity(sf::Vector2f s) : RectEntity(s)
{

}

IsoRectEntity::~IsoRectEntity()
{
    //dtor
}


void IsoRectEntity::PrepareShader(sf::Shader *shader)
{
    RectEntity::PrepareShader(shader);
    if(shader != nullptr
    && m_texture != nullptr)
    {
        shader->setUniform("normalProjMat",sf::Glsl::Mat3(IdMat3X3));
        shader->setUniform("isoToCartZFactor",0);
    }
}



}
