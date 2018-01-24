#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <SFML/Graphics.hpp>
#include "ALAGE/Types.h"
#include "ALAGE/gfx/SceneEntity.h"

namespace alag{

class SceneManager;

class SceneNode
{
    public:
        SceneNode(const NodeTypeID&);
        SceneNode(const NodeTypeID&, SceneNode* parent);
        SceneNode(const NodeTypeID&, SceneNode* parent, SceneManager* sceneManager);
        virtual ~SceneNode();

        void AddChildNode(SceneNode*);
        void AddChildNode(const NodeTypeID &id, SceneNode*);

        SceneNode* RemoveChildNode(SceneNode*);
        SceneNode* RemoveChildNode(const NodeTypeID &id);

        SceneNode* CreateChildNode();
        SceneNode* CreateChildNode(sf::Vector2f );
        SceneNode* CreateChildNode(const NodeTypeID &id);

        bool DestroyChildNode(SceneNode*);
        bool DestroyChildNode(const NodeTypeID &id);

        void RemoveAndDestroyAllChilds(bool destroyNonCreatedChilds = false);

        SceneNodeIterator GetChildIterator();

        void AttachObject(SceneObject *);
        void DetachObject(SceneObject *);
        void DetachAllEntities();
        SceneEntityIterator GetEntityIterator();

        void Move(float, float);
        void Move(float, float, float);
        void Move(sf::Vector2f );
        void Move(sf::Vector3f );
        void SetPosition(float, float);
        void SetPosition(float, float, float);
        void SetPosition(sf::Vector2f );
        void SetPosition(sf::Vector3f );

        sf::Vector3f GetGlobalPosition();
        sf::Vector3f GetPosition();

        sf::FloatRect GetGlobalBounds();
        sf::FloatRect GetBounds();

        const NodeTypeID& GetID();
        SceneNode* GetParent();
        SceneManager*  GetSceneManager();

        void Update();

    protected:
        void SetParent(SceneNode *);
        void SetSceneManager(SceneManager *);
        void SetID(const NodeTypeID &);
        NodeTypeID GenerateID();

        sf::Vector3f m_position;

        SceneManager* m_sceneManager;

    private:
        NodeTypeID m_id;
        SceneNode *m_parent;
        std::map<NodeTypeID, SceneNode*> m_childs;
        std::list<NodeTypeID> m_createdChildsList;

        std::list<SceneObject *> m_attachedObjects;
        std::list<SceneEntity *> m_entities;
        std::list<Light *> m_lights;

        int m_curNewId;
};

}

#endif // SCENENODE_H
