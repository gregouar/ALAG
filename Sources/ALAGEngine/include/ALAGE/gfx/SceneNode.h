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
        SceneNode(SceneNode* parent, const NodeTypeID&);
        virtual ~SceneNode();

        void AddChildNode();
        void AddChildNode(const NodeTypeID &id);

        SceneNode* RemoveChildNode(SceneNode*);
        SceneNode* RemoveChildNode(const NodeTypeID &id);

        SceneNode* CreateChildNode();
        SceneNode* CreateChildNode(const NodeTypeID &id);

        SceneNode* DestroyChildNode(SceneNode*);
        SceneNode* DestroyChildNode(const NodeTypeID &id);

        void RemoveAndDestroyAll(bool destroyNonCreatedChilds = false);

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
        const NodeTypeID& GenerateID();

        void Update();

    protected:
        void SetParent(SceneNode *);
        size_t FindChildCreated(const NodeTypeID&);

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
