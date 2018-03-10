#include "ALAGE/gfx/SceneObject.h"

namespace alag
{

SceneObject::SceneObject()
{
    m_parentNode = nullptr;
    m_isALight          = false;
    m_isAnEntity        = false;
    m_isAShadowCaster   = false;
    m_isVisible = true;
}

SceneObject::~SceneObject()
{
    //dtor
}


SceneNode* SceneObject::SetParentNode(SceneNode *newParent)
{
    SceneNode* oldParent = GetParentNode();
    m_parentNode = newParent;
    return oldParent;
}

SceneNode* SceneObject::GetParentNode()
{
    return m_parentNode;
}


bool SceneObject::IsALight()
{
    return m_isALight;
}

bool SceneObject::IsAnEntity()
{
    return m_isAnEntity;
}

bool SceneObject::IsAShadowCaster()
{
    return m_isAShadowCaster;
}

bool SceneObject::IsVisible()
{
    return m_isVisible;
}

void SceneObject::SetVisible(bool visible)
{
    m_isVisible = visible;
}

void SceneObject::Update(const sf::Time &elapsed_time)
{

}


void SceneObject::Notify(NotificationSender* sender, NotificationType type)
{
    if(sender == (NotificationSender*)m_parentNode)
    {
        if(type == Notification_SceneNodeDetroyed)
            m_parentNode = nullptr;
    }
}



}
