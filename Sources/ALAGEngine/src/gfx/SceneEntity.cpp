#include "ALAGE/gfx/SceneEntity.h"
#include "ALAGE/gfx/SceneNode.h"

namespace alag
{

SceneEntity::SceneEntity()
{
    m_parentNode = nullptr;
}

SceneEntity::~SceneEntity()
{
    //dtor
}

SceneNode* SceneEntity::GetParentNode()
{
    return m_parentNode;
}

SceneNode* SceneEntity::SetParentNode(SceneNode *newParent)
{
    SceneNode* oldParent = GetParentNode();
    m_parentNode = newParent;
    return oldParent;
}


}
