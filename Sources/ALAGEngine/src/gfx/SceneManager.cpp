#include "ALAGE/gfx/SceneManager.h"

#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneManager::SceneManager() : m_rootNode(0,nullptr, this)
{
    m_rootNode.SetPosition(sf::Vector3f(0,0,0));
    m_curNewId = 0;
    m_needToUpdateRenderQueue = false;
    m_last_target = nullptr;
    m_currentCamera = nullptr;

    m_ambientLight = sf::Color::White;
}

SceneManager::~SceneManager()
{
    CleanAll();
}

void SceneManager::CleanAll()
{
    m_rootNode.RemoveAndDestroyAllChilds();
    DestroyAllCreatedObjects();
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
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::RenderStates state;
        state.transform = sf::Transform::Identity;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        state.transform.translate(globalPos.x, globalPos.y);

        (*renderIt)->Render(w,state);
    }
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
           // if(entityIt.GetElement()->IsRenderable())
                m_renderQueue.push_back(entityIt.GetElement());
            ++entityIt;
        }

        SceneNodeIterator nodeIt = curNode->GetChildIterator();
        while(!nodeIt.IsAtTheEnd())
        {
            AddToRenderQueue(nodeIt.GetElement());
            ++nodeIt;
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
    AddCreatedObject(GenerateObjectID(), e);
    return e;
}

SpriteEntity* SceneManager::CreateSpriteEntity(sf::Vector2i spriteSize)
{
    return CreateSpriteEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

SpriteEntity* SceneManager::CreateSpriteEntity(sf::IntRect textureRect)
{
    SpriteEntity *e = new SpriteEntity(textureRect);
    AddCreatedObject(GenerateObjectID(), e);
    return e;
}

Light* SceneManager::CreateLight(LightType type, sf::Vector3f direction, sf::Color color)
{
    Light* light = new Light();
    light->SetType(type);
    light->SetDirection(direction);
    light->SetDiffuseColor(color);
    AddCreatedObject(GenerateObjectID(), light);
    return light;
}

Camera* SceneManager::CreateCamera(sf::Vector2f viewSize)
{
    Camera* camera = new Camera();
    camera->SetSize(viewSize);
    AddCreatedObject(GenerateObjectID(), camera);
    return camera;
}


void SceneManager::AddCreatedObject(const ObjectTypeID &id, SceneObject* obj)
{
    std::map<ObjectTypeID, SceneObject*>::iterator entityIt;
    entityIt = m_createdObjects.find(id);

    if(entityIt != m_createdObjects.end())
    {
        std::ostringstream warn_report;
        warn_report << "Adding scene object of same id as another one (ID="<<id<<")";
        Logger::Warning(warn_report);
    }

    m_createdObjects[id] = obj;
}


void SceneManager::DestroyCreatedObject(const ObjectTypeID &id)
{
    std::map<ObjectTypeID, SceneObject*>::iterator objIt;
    objIt = m_createdObjects.find(id);

    if(objIt == m_createdObjects.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy scene object (ID="<<id<<")";
        Logger::Error(error_report);
    } else {
        if(objIt->second != nullptr)
            delete objIt->second;
        m_createdObjects.erase(objIt);
    }
}

void SceneManager::DestroyAllCreatedObjects()
{
    while(!m_createdObjects.empty())
        DestroyCreatedObject(m_createdObjects.begin()->first);
}

sf::View SceneManager::GenerateView(Camera* cam)
{
    sf::View v;
    if(cam != nullptr)
    {
        v.setSize(cam->GetSize()*cam->GetZoom());
        SceneNode *node = cam->GetParentNode();
        if(node != nullptr)
        {
            sf::Vector3f globalPos = node->GetGlobalPosition();
            v.setCenter(globalPos.x, globalPos.y);
        }
    }
    return v;
}

sf::Vector2f SceneManager::ConvertMouseToScene(sf::Vector2i mouse)
{
    sf::Vector2f scenePos = sf::Vector2f(mouse);
    if(m_last_target != nullptr && m_currentCamera != nullptr)
    {
        sf::View oldView = m_last_target->getView();
        m_last_target->setView(GenerateView(m_currentCamera));
        scenePos = m_last_target->mapPixelToCoords(mouse);
        m_last_target->setView(oldView);
    }
    return scenePos;
}

void SceneManager::SetCurrentCamera(Camera *cam)
{
    m_currentCamera = cam;
}

void SceneManager::SetAmbientLight(sf::Color light)
{
    m_ambientLight = light;
    m_ambientLight.a = 255;
}


ObjectTypeID SceneManager::GenerateObjectID()
{
    return m_curNewId++;
}


}
