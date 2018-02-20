
#ifndef   ALAG_TYPES
#define   ALAG_TYPES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "AlAGE/utils/MapIterator.h"
#include "AlAGE/utils/ListIterator.h"


namespace alag
{

class SceneObject;
class SceneEntity;
class SceneNode;
class Light;
class ShadowCaster;

enum AssetLoadType
{
    LoadType_Now = 0,
    LoadType_InThread = 1,
};

enum AssetLoadSource
{
    LoadSource_None,
    LoadSource_File,
    LoadSource_Memory,
    LoadSource_Stream,
};

enum NotificationType
{
    Notification_SenderDestroyed,
    Notification_AssetLoaded,
    Notification_SceneNodeDetroyed,
    Notification_SceneNodeMoved,
   // Notification_LightMoved,
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

enum ShadowVolumeType
{
    OneSidedShadow,
    TwoSidedShadow,
    MirroredTwoSidedShadow,
};

enum PBRScreenType
{
    PBRAlbedoScreen = 0,
    PBRNormalScreen,
    PBRDepthScreen,
    PBRMaterialScreen,
};


typedef unsigned int AssetTypeID;
typedef unsigned int NodeTypeID;
typedef unsigned int ObjectTypeID;
//typedef unsigned int RenderTargetTypeID;
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
