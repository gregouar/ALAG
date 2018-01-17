#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/Types.h"
#include <vector>

namespace alag
{

class AnimatedSprite
{
    public:
        AnimatedSprite();
        virtual ~AnimatedSprite();

    protected:

    private:
        std::map<AnimationTypeID, std::vector<TextureAsset*> > m_animations;
};

}

#endif // ANIMATEDSPRITE_
