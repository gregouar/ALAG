
#ifndef   ALAG_TYPES
#define   ALAG_TYPES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "AlAGE/utils/MapIterator.h"
#include "AlAGE/utils/ListIterator.h"


#define PI 3.14159265

namespace alag
{

class SceneObject;
class SceneEntity;
class SceneNode;
class Light;
class ShadowCaster;

enum AssetLoadType
{
    LoadTypeNow = 0,
    LoadTypeInThread = 1,
};

enum AssetLoadSource
{
    NoLoadSource,
    LoadSourceFile,
    LoadSourceMemory,
    LoadSourceStream,
};

enum NotificationType
{
    NotificationSenderDestroyed,
    AssetLoadedNotification,
};

enum LightType
{
    OmniLight,
    DirectionnalLight,
    SpotLight,
};

enum ShadowCastingType
{
    NoShadow,
    DirectionnalShadow,
    DynamicShadow,
    AllShadows,
};


typedef unsigned int AssetTypeID;
typedef unsigned int NodeTypeID;
typedef unsigned int ObjectTypeID;
typedef std::string AnimationTypeID;

typedef ListIterator<SceneObject*> SceneObjectIterator;
typedef ListIterator<SceneEntity*> SceneEntityIterator;
typedef ListIterator<ShadowCaster*> ShadowCasterIterator;
typedef ListIterator<Light*> LightIterator;
typedef MapIterator<NodeTypeID, SceneNode*> SceneNodeIterator;

//typedef GLfloat Mat3x3[9];

const std::string emptyString;
const sf::Texture emptyTexture;



}

#endif
