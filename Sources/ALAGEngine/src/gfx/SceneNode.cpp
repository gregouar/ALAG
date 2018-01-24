#include "ALAGE/gfx/SceneNode.h"

#include "ALAGE/gfx/SceneManager.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

SceneNode::SceneNode(const NodeTypeID &id) : SceneNode(id, nullptr)
{
}

SceneNode::SceneNode(const NodeTypeID &id, SceneNode *p)
{
    m_sceneManager = nullptr;
    m_parent = p;

    if(m_parent != nullptr)
        SetSceneManager(m_parent->GetSceneManager());

    m_id = id;
    m_curNewId = 0;
}

SceneNode::SceneNode(const NodeTypeID &id, SceneNode *p, SceneManager* sceneManager) : SceneNode(id,p)
{
    SetSceneManager(sceneManager);
}

SceneNode::~SceneNode()
{
    RemoveAndDestroyAllChilds();
}


void SceneNode::AddChildNode(SceneNode* node)
{
    NodeTypeID id = GenerateID();
    AddChildNode(id, node);
    if(node != nullptr)
        node->SetID(id);
}

void SceneNode::AddChildNode(const NodeTypeID &id, SceneNode* node)
{
    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);

    if(childsIt != m_childs.end())
    {
        std::ostringstream warn_report;
        warn_report << "Adding child of same id as another one (ID="<<id<<")";
        Logger::Warning(warn_report);
    }

    m_childs[id] = node;

    if(node != nullptr)
        node->SetSceneManager(m_sceneManager);

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();
}

SceneNode* SceneNode::RemoveChildNode(const NodeTypeID &id)
{
    SceneNode* node = nullptr;

    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);

    if(childsIt == m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot remove child node (ID="<<id<<")";
        Logger::Error(error_report);

        return (nullptr);
    }

    node = childsIt->second;

    if(FindChildCreated(id) != m_createdChildsList.size())
        Logger::Warning("Removing created child without destroying it");

    m_childs.erase(childsIt);

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();

    return node;
}

SceneNode* SceneNode::RemoveChildNode(SceneNode* node)
{
    if(node != nullptr && node->GetParent() == this)
        return RemoveChildNode(node->GetID());
    return (nullptr);
}

SceneNode* SceneNode::CreateChildNode()
{
    return CreateChildNode(GenerateID());
}

SceneNode* SceneNode::CreateChildNode(sf::Vector2f p)
{
    SceneNode* newNode = CreateChildNode(GenerateID());
    if(newNode != nullptr)
        newNode->SetPosition(p);
    return newNode;
}

SceneNode* SceneNode::CreateChildNode(const NodeTypeID &id)
{
    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);
    if(childsIt != m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot create new child node with the same ID as an existing child node (ID="<<id<<")";
        Logger::Error(error_report);

        return childsIt->second;
    }

    SceneNode* newNode = new SceneNode(id, this);
    m_childs[id] = newNode;
    m_createdChildsList.push_back(id);
    newNode->SetSceneManager(m_sceneManager);

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();

    return newNode;
}

bool SceneNode::DestroyChildNode(SceneNode* node)
{
    if(node != nullptr && node->GetParent() == this)
        return DestroyChildNode(node->GetID());
    return (nullptr);
}

bool SceneNode::DestroyChildNode(const NodeTypeID& id)
{
    size_t foundedCreatedChild = FindChildCreated(id);
    if(foundedCreatedChild == m_createdChildsList.size())
        Logger::Warning("Destroying non-created child");
    else
        m_createdChildsList.erase(m_createdChildsList.begin() + foundedCreatedChild);

    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);
    if(childsIt == m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy child (ID="<<id<<")";
        Logger::Error(error_report);

        return (false);
    }

    if(childsIt->second != nullptr)
        delete childsIt->second;
    RemoveChildNode(id);

    return (true);
}

void SceneNode::RemoveAndDestroyAllChilds(bool destroyNonCreatedChilds)
{
    std::map<NodeTypeID, SceneNode*>::iterator childsIt;

    if(!destroyNonCreatedChilds)
        while(!m_createdChildsList.empty())
            DestroyChildNode(m_createdChildsList.back());

    if(destroyNonCreatedChilds)
    {
        childsIt = m_childs.begin();
        for(;childsIt != m_childs.end() ; ++childsIt)
        {
            if(childsIt->second != nullptr)
            {
                childsIt->second->RemoveAndDestroyAllChilds(destroyNonCreatedChilds);
                delete childsIt->second;
            }
        }
    }

    m_childs.clear();
    m_createdChildsList.clear();

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();
}

SceneNodeIterator SceneNode::GetChildIterator()
{
    return SceneNodeIterator(m_childs.begin(), m_childs.end());
}


void SceneNode::AttachEntity(SceneEntity *e)
{
    if(e != nullptr)
    {
        m_entities.push_back(e);
        if(e->SetParentNode(this) != nullptr)
            Logger::Warning("Attaching entity which has already a parent node");
    } else
        Logger::Error("Cannot attach null entity");

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();
}

SceneEntityIterator SceneNode::GetEntityIterator()
{
    return SceneEntityIterator(m_entities.begin(), m_entities.end());
}

void SceneNode::Move(float x, float y)
{
    Move(x,y,0);
}

void SceneNode::Move(float x, float y, float z)
{
    Move(sf::Vector3f(x,y,z));
}

void SceneNode::Move(sf::Vector2f p)
{
    Move(sf::Vector3f(p.x,p.y,0));
}

void SceneNode::Move(sf::Vector3f p)
{
    sf::Vector3f newPos = GetPosition();
    newPos += p;
    SetPosition(newPos);
}


void SceneNode::SetPosition(float x, float y)
{
    SetPosition(sf::Vector2f(x,y));
}

void SceneNode::SetPosition(float x, float y, float z)
{
    SetPosition(sf::Vector3f(x, y, z));
}

void SceneNode::SetPosition(sf::Vector2f xyPos)
{
    SetPosition(sf::Vector3f(xyPos.x, xyPos.y,GetPosition().z));
}

void SceneNode::SetPosition(sf::Vector3f pos)
{
    m_position = pos;

    if(m_sceneManager != nullptr)
        m_sceneManager->AskToComputeRenderQueue();
}

sf::Vector3f SceneNode::GetPosition()
{
    return m_position;
}

sf::Vector3f SceneNode::GetGlobalPosition()
{
    if(m_parent != nullptr)
        return m_parent->GetGlobalPosition() + GetPosition();
    return GetPosition();
}


const NodeTypeID& SceneNode::GetID()
{
    return m_id;
}

SceneManager* SceneNode::GetSceneManager()
{
    return m_sceneManager;
}

SceneNode* SceneNode::GetParent()
{
    return m_parent;
}



void SceneNode::SetID(const NodeTypeID &id)
{
    m_id = id;
}

void SceneNode::SetSceneManager(SceneManager *sceneManager)
{
    m_sceneManager = sceneManager;
    SceneNodeIterator childIt = GetChildIterator();
    while(!childIt.IsAtTheEnd())
    {
        SceneNode *curChild = childIt.GetElement();
        if(curChild != nullptr)
            curChild->SetSceneManager(sceneManager);
        ++childIt;
    }
}

void SceneNode::SetParent(SceneNode *p)
{
    m_parent = p;
}

NodeTypeID SceneNode::GenerateID()
{
    return m_curNewId++;
}

size_t SceneNode::FindChildCreated(const NodeTypeID& id)
{
    size_t i = 0;
    while(i < m_createdChildsList.size() && m_createdChildsList[i] != id){++i;}
    return i;
}

}
