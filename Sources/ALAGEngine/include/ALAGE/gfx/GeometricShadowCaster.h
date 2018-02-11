#ifndef GEOMETRICSHADOWCASTER_H
#define GEOMETRICSHADOWCASTER_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/ShadowCaster.h"
#include "ALAGE/gfx/SceneObject.h"


namespace alag
{

class GeometricShadowCaster : public ShadowCaster
{
    public:
        GeometricShadowCaster();
        virtual ~GeometricShadowCaster();

        bool DoRequireShadowCasting();
        virtual void ComputeShadow(Light*);

        virtual void RenderShadow(sf::RenderTarget*, Light*);

        void ConstructCube(float x, float y, float z);
        void SetGeometry(sf::VertexArray , float height);

    protected:
        sf::VertexArray m_geometry;
        float m_height;

    private:
};

}

#endif // SPRITEENTITY_H
