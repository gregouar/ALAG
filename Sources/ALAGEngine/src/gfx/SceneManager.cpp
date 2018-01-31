#include "ALAGE/gfx/SceneManager.h"

#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Mathematics.h"

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


bool SceneManager::InitRenderer(sf::Vector2u windowSize)
{
    return (true);
}

void SceneManager::Update(sf::Time elapsedTime)
{
    m_rootNode.Update(elapsedTime);

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
    //AddToRenderQueue(&m_rootNode);
    m_rootNode.SearchInsideForEntities(&m_renderQueue);
}


int SceneManager::UpdateLighting(std::multimap<float, Light*> &lightList, int maxNbrLights)
{
    int curNbrLights = 0;

    std::multimap<float, Light*>::iterator lightIt;
    for(lightIt = lightList.begin() ; lightIt != lightList.end() && curNbrLights < maxNbrLights ; ++lightIt)
    {
        Light* curLight = lightIt->second;
        SceneNode* node = curLight->GetParentNode();

        if(node != nullptr)
        {
            sf::Vector3f pos(0,0,0);
            GLfloat glPos[] = { 0,0,0,1.0 };
            GLfloat glDirection[] = { 0,0,1 };
            GLfloat glColor[] = {1,1,1,1};

            if(curLight->GetType() == DirectionnalLight)
            {
                pos = curLight->GetDirection();
                glPos[3] = 0;
            } else
                pos = node->GetGlobalPosition();

            glPos[0] = pos.x;
            glPos[1] = pos.y;
            glPos[2] = pos.z;

            glLightfv(GL_LIGHT0+curNbrLights, GL_POSITION, glPos);
            SfColorToGlColor(curLight->GetDiffuseColor(), glColor);
            glLightfv(GL_LIGHT0+curNbrLights, GL_DIFFUSE, glColor);
            SfColorToGlColor(curLight->GetSpecularColor(), glColor);
            glLightfv(GL_LIGHT0+curNbrLights, GL_SPECULAR, glColor);
            glLightf(GL_LIGHT0+curNbrLights, GL_CONSTANT_ATTENUATION, curLight->GetConstantAttenuation());
            glLightf(GL_LIGHT0+curNbrLights, GL_LINEAR_ATTENUATION, curLight->GetLinearAttenuation());
            glLightf(GL_LIGHT0+curNbrLights, GL_QUADRATIC_ATTENUATION, curLight->GetQuadraticAttenuation());

            pos = curLight->GetDirection();
            glDirection[0] = pos.x;
            glDirection[1] = pos.y;
            glDirection[2] = pos.z;
            glLightfv(GL_LIGHT0+curNbrLights, GL_SPOT_DIRECTION, glDirection);

            if(curLight->IsCastShadowEnabled())
            {
                curLight->GetShadowCasterList()->clear();
                node->FindNearbyShadowCaster(curLight->GetShadowCasterList(),curLight->GetType());
                curLight->UpdateShadow();
            }

            ++curNbrLights;
        }
    }

    return curNbrLights;
}



void SceneManager::RenderShadows(std::multimap<float, Light*> &lightList,const sf::View &view,
                                 const sf::Vector2u &screen_size, int maxNbrLights)
{
    int curNbrLights = 0;

    std::multimap<float, Light*>::iterator lightIt;
    for(lightIt = lightList.begin() ; lightIt != lightList.end()
    && curNbrLights < maxNbrLights ; ++lightIt)
    {
        Light* curLight = lightIt->second;
        SceneNode* node = curLight->GetParentNode();

        if(node != nullptr)
        {
            if(curLight->IsCastShadowEnabled())
                curLight->RenderShadowMap(view,screen_size);

            ++curNbrLights;
        }
    }
}


/*void SceneManager::AddToRenderQueue(SceneNode *curNode)
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
}*/

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
