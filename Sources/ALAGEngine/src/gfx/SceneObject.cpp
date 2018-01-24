#include "ALAGE/gfx/SceneObject.h"

namespace alag
{

SceneObject::SceneObject()
{
    m_parentNode = nullptr;
    m_isALight = false;
    m_isAnEntity = false;
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


}
