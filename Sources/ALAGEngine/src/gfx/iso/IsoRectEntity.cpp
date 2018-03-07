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
    m_scene = nullptr;
    m_enableParallax = false;
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
        shader->setUniform("p_normalProjMat",sf::Glsl::Mat3(IdMat3X3));
        shader->setUniform("enable_parallax",m_enableParallax);
        shader->setUniform("texture_size",sf::Vector2f(m_texture->GetTexture()->getSize()));
       // shader->setUniform("p_isoToCartZFactor",0);

    }
}

void IsoRectEntity::SetParallax(bool p)
{
    m_enableParallax = p;
}

void IsoRectEntity::SetIsoScene(PBRIsoScene *scene)
{
    m_scene = scene;
}


PBRIsoScene* IsoRectEntity::GetIsoScene()
{
    return m_scene;
}


}
