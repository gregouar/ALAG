
#ifndef   ALAG_TYPES
#define   ALAG_TYPES

#include <iostream>

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
typedef std::string AnimationTypeID;


#endif
