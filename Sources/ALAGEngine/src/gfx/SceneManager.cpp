#include "ALAGE/gfx/SceneManager.h"

namespace alag
{

SceneManager::SceneManager() : m_rootNode(0)
{
    m_rootNode.SetPosition(sf::Vector3f(0,0,0));
}

SceneManager::~SceneManager()
{
    //dtor
}


SceneNode *SceneManager::GetRootNode()
{
    return &m_rootNode;
}

RectEntity* SceneManager::CreateRectEntity(sf::FloatRect rect)
{
    RectEntity *e = new RectEntity(rect);
    AddEntity(e);
    return e;
}

}
