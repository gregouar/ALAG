
#ifndef   ALAG_TYPES
#define   ALAG_TYPES

#include <iostream>
#include <SFML/Graphics.hpp>
#include "AlAGE/utils/MapIterator.h"
#include "AlAGE/utils/ListIterator.h"


#define PI 3.14159265

namespace alag
{

class SceneEntity;
class SceneNode;

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


typedef unsigned int AssetTypeID;
typedef unsigned int NodeTypeID;
typedef unsigned int EntityTypeID;
typedef std::string AnimationTypeID;

typedef ListIterator<SceneEntity*> SceneEntityIterator;
typedef MapIterator<NodeTypeID, SceneNode*> SceneNodeIterator;

const std::string emptyString;
const sf::Texture emptyTexture;


}

#endif
