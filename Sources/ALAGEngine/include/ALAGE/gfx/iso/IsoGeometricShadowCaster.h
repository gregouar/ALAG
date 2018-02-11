#ifndef ISOGEOMETRICSHADOWCASTER_H
#define ISOGEOMETRICSHADOWCASTER_H

#include "ALAGE/gfx/GeometricShadowCaster.h"

namespace alag
{

class PBRIsoScene;

class IsoGeometricShadowCaster : public GeometricShadowCaster
{
    friend class PBRIsoScene;

    public:
        IsoGeometricShadowCaster();
        virtual ~IsoGeometricShadowCaster();

        void RenderShadow(sf::RenderTarget*, Light*);

    protected:
        void SetIsoScene(PBRIsoScene *scene);

    private:
        PBRIsoScene *m_scene;
};

}



#endif // ISOGEOMETRICSHADOWCASTER_H

