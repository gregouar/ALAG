#include "ALAGE/gfx/SceneManager.h"

#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneManager::SceneManager() : m_rootNode(0)
{
    m_rootNode.SetPosition(sf::Vector3f(0,0,0));
    m_curNewId = 0;
}

SceneManager::~SceneManager()
{
    CleanAll();
}

void SceneManager::CleanAll()
{
    m_rootNode.RemoveAndDestroyAllChilds();
    DestroyAllEntities();
}

SceneNode *SceneManager::GetRootNode()
{
    return &m_rootNode;
}

RectEntity* SceneManager::CreateRectEntity(sf::FloatRect rect)
{
    RectEntity *e = new RectEntity(rect);
    AddEntity(GenerateEntityID(), e);
    return e;
}


void SceneManager::AddEntity(const EntityTypeID &id, SceneEntity* entity)
{
    std::map<EntityTypeID, SceneEntity*>::iterator entityIt;
    entityIt = m_entities.find(id);

    if(entityIt != m_entities.end())
    {
        std::ostringstream warn_report;
        warn_report << "Adding entity of same id as another one (ID="<<id<<")";
        Logger::Warning(warn_report);
    }

    m_entities[id] = entity;
}


void SceneManager::DestroyEntity(const EntityTypeID &id)
{
    std::map<EntityTypeID, SceneEntity*>::iterator entityIt;
    entityIt = m_entities.find(id);

    if(entityIt == m_entities.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy entity (ID="<<id<<")";
        Logger::Error(error_report);
    } else {
        if(entityIt->second != nullptr)
            delete entityIt->second;
        m_entities.erase(entityIt);
    }
}

void SceneManager::DestroyAllEntities()
{
    std::map<EntityTypeID, SceneEntity*>::iterator entityIt;
    for(entityIt = m_entities.begin() ;  entityIt != m_entities.end() ; entityIt++)
        DestroyEntity(entityIt->first);
}

EntityTypeID SceneManager::GenerateEntityID()
{
    return m_curNewId++;
}


}
