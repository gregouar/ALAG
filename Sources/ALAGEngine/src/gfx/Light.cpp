#include "ALAGE/gfx/Light.h"

namespace alag
{

Light::Light()
{
    m_isALight = true;
    m_direction = sf::Vector3f(0,0,-1);
    m_diffuseColor = sf::Color::White;
    m_specularColor = sf::Color::White;
    m_constantAttenuation = 1;
    m_linearAttenuation = 0;
    m_quadraticAttenuation = 0;
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

sf::Color Light::GetSpecularColor()
{
    return m_specularColor;
}

float Light::GetConstantAttenuation()
{
    return m_constantAttenuation;
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

void Light::SetSpecularColor(sf::Color color)
{
    m_specularColor = color;
}

void Light::SetConstantAttenuation(float constAtt)
{
    if(constAtt >= 1)
        m_constantAttenuation = constAtt;
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
