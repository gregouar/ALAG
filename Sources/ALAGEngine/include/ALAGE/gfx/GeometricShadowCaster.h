#ifndef GEOMETRICSHADOWCASTER_H
#define GEOMETRICSHADOWCASTER_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/ShadowCaster.h"
#include "ALAGE/gfx/SceneObject.h"


namespace alag
{

class GeometricShadowCaster : public SceneObject, public ShadowCaster
{
    public:
        GeometricShadowCaster();
        virtual ~GeometricShadowCaster();

        bool DoRequireShadowCasting();

    protected:

    private:
};

}

#endif // SPRITEENTITY_H
