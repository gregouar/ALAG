#include "ALAGE/gfx/Light.h"

namespace alag
{

Light::Light()
{
    m_isALight = true;
    m_direction = sf::Vector3f(0,0,-1);
    m_diffuseColor = sf::Color::White;
    m_linearAttenuation = 1;
    m_quadraticAttenuation = 1;
}

Light::~Light()
{
    //dtor
}

LightType Light::GetType()
{
    return m_type;
}

sf::Vector3f Light::GetDirection()
{
    return m_direction;
}

sf::Color Light::GetDiffuseColor()
{
    return m_diffuseColor;
}

float Light::GetLinearAttenuation()
{
    return m_linearAttenuation;
}

float Light::GetQuadraticAttenuation()
{
    return m_quadraticAttenuation;
}


void Light::SetType(LightType type)
{
    m_type = type;
}

void Light::SetDirection(sf::Vector3f direction)
{
    m_direction = direction;
}

void Light::SetDiffuseColor(sf::Color color)
{
    m_diffuseColor = color;
}

void Light::SetLinearAttunation(float linAtt)
{
    m_linearAttenuation = linAtt;
}

void Light::SetQuadraticAttenuation(float quadAtt)
{
    m_quadraticAttenuation = quadAtt;
}


}
