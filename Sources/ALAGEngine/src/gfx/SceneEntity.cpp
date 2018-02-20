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
    m_isStatic = false;
    m_usePBR = false;
    m_ZDepth = 0.5;
    m_isAShadowCaster = true;

    m_isAskingForRenderUpdate = false;
}

SceneEntity::~SceneEntity()
{
    //dtor
}

bool SceneEntity::CanBeLighted()
{
    return m_isLighted & m_canBeLighted;
}

bool SceneEntity::UsePBR()
{
    return m_usePBR;
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

bool SceneEntity::IsStatic()
{
    return m_isStatic;
}

bool SceneEntity::IsAskingForRenderUpdate()
{
    return m_isAskingForRenderUpdate;
}

float SceneEntity::GetZDepth()
{
    return (m_ZDepth);
}

void SceneEntity::AskForRenderUpdate(bool ask)
{
    m_isAskingForRenderUpdate = ask;
}

void SceneEntity::SetStatic(bool s)
{
    m_isStatic = s;
}

void SceneEntity::SetZDepth(float z)
{
    if(z >= 0 && z <= 1)
        m_ZDepth = z;
}



void SceneEntity::Notify(NotificationSender* sender, NotificationType type)
{
    if(sender == m_parentNode)
    {
        if(type == Notification_SceneNodeMoved)
            AskForRenderUpdate();
    }

    ShadowCaster::Notify(sender, type);
}

}
