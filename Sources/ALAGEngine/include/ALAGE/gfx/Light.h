#ifndef LIGHT_H
#define LIGHT_H

#include "ALAGE/Types.h"
#include "ALAGE/gfx/SceneObject.h"

namespace alag
{

class Light : public SceneObject
{
    public:
        Light();
        virtual ~Light();

        LightType GetType();
        sf::Vector3f GetDirection();
        sf::Color GetDiffuseColor();
        float GetLinearAttenuation();
        float GetQuadraticAttenuation();

        void SetType(LightType);
        void SetDirection(sf::Vector3f);
        void SetDiffuseColor(sf::Color);
        void SetLinearAttunation(float);
        void SetQuadraticAttenuation(float);

    protected:

    private:
        LightType    m_type;
        sf::Vector3f m_direction;
        sf::Color m_diffuseColor;
        float m_linearAttenuation;
        float m_quadraticAttenuation;
};

}

#endif // LIGHTENTITY_H
