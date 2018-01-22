#include "ALAGE/gfx/SceneManager.h"

#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneManager::SceneManager() : m_rootNode(0,nullptr, this)
{
    m_rootNode.SetPosition(sf::Vector3f(0,0,0));
    m_curNewId = 0;
    m_needToUpdateRenderQueue = false;
    m_view.setCenter(0,0);
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

void SceneManager::Update(sf::Time elapsedTime)
{
    if(m_needToUpdateRenderQueue)
    {
        ComputeRenderQueue();
        m_needToUpdateRenderQueue = false;
    }
}

void SceneManager::AskToComputeRenderQueue()
{
    m_needToUpdateRenderQueue = true;
}

void SceneManager::ProcessRenderQueue(sf::RenderTarget *w)
{
    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); renderIt++)
        (*renderIt)->Render(w);
}

void SceneManager::ComputeRenderQueue()
{
    m_renderQueue.clear();
    AddToRenderQueue(&m_rootNode);
}

void SceneManager::AddToRenderQueue(SceneNode *curNode)
{
    if(curNode != nullptr)
    {
        SceneEntityIterator entityIt = curNode->GetEntityIterator();
        while(!entityIt.IsAtTheEnd())
        {
            m_renderQueue.push_back(entityIt.GetElement());
            entityIt++;
        }

        SceneNodeIterator nodeIt = curNode->GetChildIterator();
        while(!nodeIt.IsAtTheEnd())
        {
            AddToRenderQueue(nodeIt.GetElement());
            nodeIt++;
        }
    }
}

SceneNode *SceneManager::GetRootNode()
{
    return &m_rootNode;
}

RectEntity* SceneManager::CreateRectEntity(sf::Vector2f rectSize)
{
    RectEntity *e = new RectEntity(rectSize);
    AddEntity(GenerateEntityID(), e);
    return e;
}


SpriteEntity* SceneManager::CreateSpriteEntity(sf::Vector2i spriteSize)
{
    return CreateSpriteEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

SpriteEntity* SceneManager::CreateSpriteEntity(sf::IntRect textureRect)
{
    SpriteEntity *e = new SpriteEntity(textureRect);
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

void SceneManager::MoveView(sf::Vector2f m)
{
    m_view.move(m);
}

EntityTypeID SceneManager::GenerateEntityID()
{
    return m_curNewId++;
}


}
