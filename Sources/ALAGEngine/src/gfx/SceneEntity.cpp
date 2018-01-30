#include "ALAGE/gfx/SceneEntity.h"
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneEntity::SceneEntity()
{
    m_isAnEntity = true;
    m_canBeLighted = false;
    m_isLighted = true;
    m_is3D = false;
    m_ZDepth = 0.5;
    m_isAShadowCaster = true;
}

SceneEntity::~SceneEntity()
{
    //dtor
}

bool SceneEntity::CanBeLighted()
{
    return m_isLighted & m_canBeLighted;
}

bool SceneEntity::Is3D()
{
    return m_is3D;
}

void SceneEntity::EnableLighting()
{
    m_isLighted = true;
    if(!m_canBeLighted)
        Logger::Error("Cannot activate illumination");
}

void SceneEntity::DisableLighting()
{
    m_isLighted = false;
}

void SceneEntity::PrepareShader(sf::Shader *)
{

}

float SceneEntity::GetZDepth()
{
    return (m_ZDepth);
}

void SceneEntity::SetZDepth(float z)
{
    if(z >= 0 && z <= 1)
        m_ZDepth = z;
}

}
