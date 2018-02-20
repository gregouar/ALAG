#ifndef SCENENODE_H
#define SCENENODE_H

#include <list>
#include <SFML/Graphics.hpp>
#include "ALAGE/Types.h"
#include "ALAGE/gfx/SceneEntity.h"

namespace alag{

class DefaultScene;

class SceneNode : public NotificationSender, public NotificationListener
{
    public:
        SceneNode(const NodeTypeID&);
        SceneNode(const NodeTypeID&, SceneNode* parent);
        SceneNode(const NodeTypeID&, SceneNode* parent, DefaultScene* scene);
        virtual ~SceneNode();

        void AddChildNode(SceneNode*);
        void AddChildNode(const NodeTypeID &id, SceneNode*);

        SceneNode* RemoveChildNode(SceneNode*);
        SceneNode* RemoveChildNode(const NodeTypeID &id);

        SceneNode* CreateChildNode();
        SceneNode* CreateChildNode(float, float );
        SceneNode* CreateChildNode(float, float, float );
        SceneNode* CreateChildNode(sf::Vector2f );
        SceneNode* CreateChildNode(sf::Vector3f );
        SceneNode* CreateChildNode(const NodeTypeID &id);

        bool DestroyChildNode(SceneNode*);
        bool DestroyChildNode(const NodeTypeID &id);

        void RemoveAndDestroyAllChilds(bool destroyNonCreatedChilds = false);

        SceneNodeIterator GetChildIterator();
        SceneObjectIterator GetSceneObjectIterator();
        SceneEntityIterator GetEntityIterator();
        LightIterator GetLightIterator();
        ShadowCasterIterator GetShadowCasterIterator();

        void AttachObject(SceneObject *);
        void DetachObject(SceneObject *);
        void DetachAllEntities();

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
        DefaultScene*  GetSceneManager();

        void SearchInsideForEntities(std::list<SceneEntity*>  *renderQueue);

        void FindNearbyLights(std::multimap<float, Light*> *foundedLights);
        void SearchInsideForLights(std::multimap<float, Light*> *foundedLights, sf::Vector3f);

        void FindNearbyShadowCaster(std::list<ShadowCaster*> *foundedCaster, LightType);
        void SearchInsideForShadowCaster(std::list<ShadowCaster*> *foundedCaster, LightType);

        void Update(const sf::Time &);

        virtual void Notify(NotificationSender*, NotificationType);

    protected:
        void SetParent(SceneNode *);
        void SetSceneManager(DefaultScene *);
        void SetID(const NodeTypeID &);
        NodeTypeID GenerateID();

        sf::Vector3f m_position;
        sf::Vector3f m_globalPosition;

        DefaultScene* m_sceneManager;

    private:
        NodeTypeID m_id;
        SceneNode *m_parent;
        std::map<NodeTypeID, SceneNode*> m_childs;
        std::list<NodeTypeID> m_createdChildsList;

        std::list<SceneObject *> m_attachedObjects;
        std::list<SceneEntity *> m_entities;
        std::list<Light *> m_lights;
        std::list<ShadowCaster *> m_shadowCasters;

        int m_curNewId;
};

}

#endif // SCENENODE_H
