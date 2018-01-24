#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

namespace alag
{

class SceneNode;
class SceneEntity;
class Light;

class SceneObject
{
    friend class SceneNode;
    friend class SceneEntity; //SceneEntity::SceneEntity();
    friend class Light;//::Light();

    public:
        SceneObject();
        virtual ~SceneObject();

        SceneNode *GetParentNode();

        virtual bool IsALight();
        virtual bool IsAnEntity();

    protected:
        SceneNode* SetParentNode(SceneNode*);
        SceneNode *m_parentNode;


    private:
        bool m_isALight;
        bool m_isAnEntity;
};

}

#endif // SCENEOBJECT_H
