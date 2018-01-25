
#ifndef   ALAG_TYPES
#define   ALAG_TYPES

#include <iostream>
#include <SFML/Graphics.hpp>
#include "AlAGE/utils/MapIterator.h"
#include "AlAGE/utils/ListIterator.h"


#define PI 3.14159265

namespace alag
{

class SceneObject;
class SceneEntity;
class SceneNode;
class Light;

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


typedef unsigned int AssetTypeID;
typedef unsigned int NodeTypeID;
typedef unsigned int ObjectTypeID;
typedef std::string AnimationTypeID;

typedef ListIterator<SceneObject*> SceneObjectIterator;
typedef ListIterator<SceneEntity*> SceneEntityIterator;
typedef ListIterator<Light*> LightIterator;
typedef MapIterator<NodeTypeID, SceneNode*> SceneNodeIterator;

const std::string emptyString;
const sf::Texture emptyTexture;


}

#endif
