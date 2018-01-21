#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <SFML/Graphics.hpp>
#include "ALAGE/Types.h"
#include "ALAGE/gfx/SceneEntity.h"

namespace alag{

class SceneNode
{
    public:
        SceneNode(const NodeTypeID&);
        SceneNode(const NodeTypeID&, SceneNode* parent);
        virtual ~SceneNode();

        void AddChildNode(SceneNode*);
        void AddChildNode(const NodeTypeID &id, SceneNode*);

        SceneNode* RemoveChildNode(SceneNode*);
        SceneNode* RemoveChildNode(const NodeTypeID &id);

        SceneNode* CreateChildNode();
        SceneNode* CreateChildNode(const NodeTypeID &id);

        bool DestroyChildNode(SceneNode*);
        bool DestroyChildNode(const NodeTypeID &id);

        void RemoveAndDestroyAllChilds(bool destroyNonCreatedChilds = false);

        void AttachEntity(SceneEntity *);
        void DetachEntity(SceneEntity *);
        void DetachAllEntities();

        void Move(sf::Vector2f );
        void Move(sf::Vector3f );
        void SetPosition(sf::Vector2f );
        void SetPosition(sf::Vector3f );

        sf::Vector3f GetGlobalPosition();
        sf::Vector3f GetPosition();

        const NodeTypeID& GetID();
        SceneNode* GetParent();

        void Update();

    protected:
        void SetParent(SceneNode *);
        void SetID(const NodeTypeID &);
        size_t FindChildCreated(const NodeTypeID&);
        NodeTypeID GenerateID();

        sf::Vector3f m_position;

    private:
        NodeTypeID m_id;
        SceneNode *m_parent;
        std::map<NodeTypeID, SceneNode*> m_childs;
        std::vector<NodeTypeID> m_createdChildsList;

        std::list<SceneEntity *> m_entities;

        int m_curNewId;
};

}

#endif // SCENENODE_H
