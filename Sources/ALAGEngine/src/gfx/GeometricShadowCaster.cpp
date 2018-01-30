#include "ALAGE/gfx/GeometricShadowCaster.h"

namespace alag
{

GeometricShadowCaster::GeometricShadowCaster()
{
    m_isAShadowCaster = true;
    ForceShadowCastingType(AllShadows);
}

GeometricShadowCaster::~GeometricShadowCaster()
{
    //dtor
}

}
