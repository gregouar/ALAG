#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <SFML/Graphics.hpp>

namespace alag
{

class SceneNode;
class SceneEntity;
class Light;

class SceneObject
{
    friend class SceneNode;
    friend class SceneEntity; //SceneEntity::SceneEntity();
    friend class GeometricShadowCaster; //SceneEntity::SceneEntity();
    friend class Light;//::Light();

    public:
        SceneObject();
        virtual ~SceneObject();

        SceneNode *GetParentNode();

        virtual bool IsALight();
        virtual bool IsAnEntity();
        virtual bool IsAShadowCaster();

        virtual void Update(const sf::Time &);

    protected:
        SceneNode* SetParentNode(SceneNode*);
        SceneNode *m_parentNode;

    private:
        bool m_isALight;
        bool m_isAnEntity;
        bool m_isAShadowCaster;
};

}

#endif // SCENEOBJECT_H
