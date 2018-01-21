#include "ALAGE/gfx/SceneNode.h"

namespace alag
{

SceneNode::SceneNode(const NodeTypeID &id) : SceneNode(id, nullptr)
{
}

SceneNode::SceneNode(const NodeTypeID &id, SceneNode *p)
{
    m_parent = p;
    m_curNewId = 0;
}

SceneNode::~SceneNode()
{
    RemoveAndDestroyAll();
}


void SceneNode::AddChildNode(SceneNode* node)
{
    AddChildNode(GenerateID(), node);
}

void SceneNode::AddChildNode(const NodeTypeID &id, SceneNode* node)
{
    m_childs[id] = node;
}

void SceneNode::RemoveChildNode(const NodeTypeID &id)
{
    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);

    if(childsIt == m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot remove child node (ID="<<id<<")";
        Logger::Error(error_report);
    }

    if(FindChildCreated(id) == m_createdChildsList.size())
        Logger::Warning("Removing created child without destroying it");

    m_childs.erase(childsIt);
}

void SceneNode::RemoveChildNode(SceneNode* node)
{
    if(node != nullptr && node->GetParent() == this)
        RemoveChildNode(node->GetID());
}

SceneNode* SceneNode::CreateChildNode()
{
    return CreateChildNode(GenerateID());
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

    SceneNode* newNode = new SceneNode(this, id);
    m_childs[id] = newNode;
    m_createdChildsList.add(newNode);
}


void SceneNode::DestroyChildNode(SceneNode* node)
{
    if(node != nullptr && node->GetParent() == this)
        DestroyChildNode(node->GetID());
}

void SceneNode::DestroyChildNode(const NodeTypeID& id)
{
    int foundedCreatedChild = FindChildCreated(id);
    if(foundedCreatedChild == m_createdChildsList.size())
        Logger::Warning("Destroying non-created child");
    else
        m_createdChildsList.erase(m_createdChildsList.begin() + foundedCreatedChild);

    std::map<NodeTypeID, SceneNode*>::iterator childsIt;
    childsIt = m_childs.find(id);
    if(childsIt != m_childs.end())
    {
        std::ostringstream error_report;
        error_report << "Cannot destroy child (ID="<<id<<")";
        Logger::Error(error_report);
    } else {
        delete m_childs[id];
        RemoveChildNode(id);
    }
}

void SceneNode::RemoveAndDestroyAll(bool destroyNonCreatedChilds)
{
    std::map<NodeTypeID, SceneNode*>::iterator childsIt;

    if(!destroyNonCreatedChilds)
        for(size_t i = 0 ; i < m_createdChildsList.size() ; i++)
        {
            childsIt = m_childs.find(m_createdChildsList[i]);
            if(childsIt != m_childs.end() && childsIt->second != nullptr)
                delete childsIt->second;
        }

    if(destroyNonCreatedChilds)
    {
        childsIt = m_childs.begin();
        for(;childsIt != m_childs.end() ; childsIt++)
        {
            if(childsIt->second != nullptr)
            {
                childsIt->second->RemoveAndDestroyAll(destroyNonCreatedChilds);
                delete childsIt->second;
            }
        }
    }

    m_childs.clear();
    m_createdChildsList.clear();
}

void SceneNode::AttachEntity(SceneEntity *e)
{
    m_entities.add(e);
}

void SceneNode::Move(sf::Vector2f p)
{
    m_position.x += p.x;
    m_position.y += p.y;
}

void SceneNode::Move(sf::Vector3f p)
{
    m_position += p;
}


void SceneNode::SetPosition(sf::Vector2f xyPos)
{
    m_position.x = xyPos.x;
    m_position.y = xyPos.y;
}

void SceneNode::SetPosition(sf::Vector3f pos)
{
    m_position = pos;
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


NodeTypeID SceneNode::GenerateID()
{
    return m_curNewId++;
}


size_t SceneNode::FindChildCreated(const NodeTypeID& id)
{
    size_t i = 0;
    while(i < m_createdChildsList.size() && m_createdChildsList[i] != id){};
    return i;
}

}
