#include "ALAGE/gfx/SceneEntity.h"
#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneEntity::SceneEntity()
{
    m_parentNode = nullptr;
    m_isRenderable = false;
    m_canBeLighted = false;
    m_isLighted = true;
    m_is3D = false;
}

SceneEntity::~SceneEntity()
{
    //dtor
}

SceneNode* SceneEntity::GetParentNode()
{
    return m_parentNode;
}

bool SceneEntity::CanBeIlluminated()
{
    return m_isLighted & m_canBeLighted;
}

bool SceneEntity::Is3D()
{
    return m_is3D;
}

bool SceneEntity::IsRenderable()
{
    return m_isRenderable;
}

void SceneEntity::ActivateLighting()
{
    m_isLighted = true;
    if(!m_canBeLighted)
        Logger::Error("Cannot activate illumination");
}

void SceneEntity::DesactivateLighting()
{
    m_isLighted = false;
}

SceneNode* SceneEntity::SetParentNode(SceneNode *newParent)
{
    SceneNode* oldParent = GetParentNode();
    m_parentNode = newParent;
    return oldParent;
}

void SceneEntity::PrepareShader(sf::Shader *)
{

}


}
