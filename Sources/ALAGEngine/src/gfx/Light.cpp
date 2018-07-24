#include "ALAGE/gfx/Light.h"
#include "ALAGE/gfx/ShadowCaster.h"
#include "ALAGE/utils/TextureModifier.h"

namespace alag
{

Light::Light()
{
    m_isALight = true;
    m_direction = sf::Vector3f(0,0,-1);
    m_diffuseColor = sf::Color::White;
    //m_specularColor = sf::Color::White;
    //m_constantAttenuation = 1;
    m_radius = 50;
    m_intensity = 1;
    m_linearAttenuation = 0;
    m_quadraticAttenuation = 1;
    m_castShadow = false;
    m_shadowMaxShift = sf::IntRect(0,0,0,0);
    m_requireShadowComputation = false;
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

/*sf::Color Light::GetSpecularColor()
{
    return m_specularColor;
}

float Light::GetConstantAttenuation()
{
    return m_constantAttenuation;
}*/

float Light::GetRadius()
{
    return m_radius;
}

float Light::GetIntensity()
{
    return m_intensity;
}

float Light::GetLinearAttenuation()
{
    return m_linearAttenuation;
}

float Light::GetQuadraticAttenuation()
{
    return m_quadraticAttenuation;
}

bool Light::IsCastShadowEnabled()
{
    return m_castShadow;
}


void Light::SetType(LightType type)
{
    m_type = type;
    m_requireShadowComputation = true;
}

void Light::SetDirection(sf::Vector3f direction)
{
    m_direction = direction;
    m_requireShadowComputation = true;
}

void Light::SetDiffuseColor(sf::Color color)
{
    m_diffuseColor = color;
}

/*void Light::SetSpecularColor(sf::Color color)
{
    m_specularColor = color;
}

void Light::SetConstantAttenuation(float constAtt)
{
    if(constAtt > 0)
        m_constantAttenuation = constAtt;
}*/

void Light::SetRadius(float radius)
{
    if(radius >= 0)
        m_radius = radius;
}

void Light::SetIntensity(float intensity)
{
    if(intensity >= 0)
        m_intensity = intensity;
}

void Light::SetLinearAttunation(float linAtt)
{
    m_linearAttenuation = linAtt;
}

void Light::SetQuadraticAttenuation(float quadAtt)
{
    if(quadAtt > 0)
        m_quadraticAttenuation = quadAtt;
}

void Light::SetShadowMapSize(sf::Vector2u s)
{
    if(m_shadowMap.getSize() != s)
        m_shadowMap.create(s.x,s.y,true);
}

void Light::SetShadowMapSize(unsigned x, unsigned y)
{
    SetShadowMapSize(sf::Vector2u(x,y));
}

void Light::EnableShadowCasting()
{
    m_castShadow = true;
    m_requireShadowComputation = true;
}

void Light::DisableShadowCasting()
{
    m_castShadow = false;
}

const sf::Texture& Light::GetShadowMap()
{
    return m_shadowMap.getTexture();
}

const sf::IntRect& Light::GetShadowMaxShift()
{
    return m_shadowMaxShift;
}

/*sf::Vector2f Light::GetShadowMapRatio()
{
    return
     sf::Vector2f(m_shadowMap_size/(float)GetShadowMap().getSize().x,
                  m_shadowMap_size/(float)GetShadowMap().getSize().y);
}*/

std::list<ShadowCaster*> *Light::GetShadowCasterList()
{
    return &m_shadowCasterList;
}

void Light::UpdateShadow()
{
    sf::IntRect max_shift(0,0,0,0);

    std::list<ShadowCaster *>::iterator casterIt;
    for(casterIt = m_shadowCasterList.begin() ; casterIt != m_shadowCasterList.end() ; ++casterIt)
    {
        if(m_requireShadowComputation || (*casterIt)->IsRequiringShadowCasting(this))
            (*casterIt)->ComputeShadow(this);

        sf::IntRect shift = (*casterIt)->GetShadowMaxShift(this);
        if(shift.left < max_shift.left)
            max_shift.left = shift.left;
        if(shift.top < max_shift.top)
            max_shift.top = shift.top;
        if(shift.width > max_shift.width)
            max_shift.width = shift.width;
        if(shift.height > max_shift.height)
            max_shift.height = shift.height;
    }

    m_shadowMaxShift = max_shift;
    m_requireShadowComputation = false;
}

void Light::RenderShadowMap(const sf::View &view/*,const sf::Vector2u &screen_size*/)
{
    /** Should probably generate this based on something else than view size, so that it does not
        recreate when zooming **/
 /*   if((int)m_shadowMap.getSize().x != (int)((view.getSize().x + m_shadowMaxShift.width)*m_shadowMap_size)
    || (int)m_shadowMap.getSize().y != (int)((view.getSize().y + m_shadowMaxShift.height)*m_shadowMap_size))
        m_shadowMap.create((int)((view.getSize().x + m_shadowMaxShift.width)*m_shadowMap_size),
                            (int)((view.getSize().y + m_shadowMaxShift.height)*m_shadowMap_size), true);*/
    if(m_shadowMap.getSize().x != 0)
    {
        sf::View shadow_view = view;
        shadow_view.move(m_shadowMaxShift.width*0.5+m_shadowMaxShift.left,
                         m_shadowMaxShift.height*0.5+m_shadowMaxShift.top);
        shadow_view.setSize(view.getSize().x+m_shadowMaxShift.width,
                            view.getSize().y+m_shadowMaxShift.height);

        m_shadowMap.setActive(true);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            m_shadowMap.clear(sf::Color::White);
            glClear(GL_DEPTH_BUFFER_BIT);

            m_shadowMap.setView(shadow_view);

            std::list<ShadowCaster *>::iterator casterIt;
            for(casterIt = m_shadowCasterList.begin() ; casterIt != m_shadowCasterList.end() ; ++casterIt)
                (*casterIt)->RenderShadow(&m_shadowMap,this);

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            //m_shadowMap.display();
            m_shadowMap.display(/*false*/);
       // m_shadowMap.setActive(false);

      // m_shadowMap.getTexture().copyToImage().saveToFile("shadowmap.png");

    }
}


void Light::Notify(NotificationSender* sender, NotificationType type)
{
    if(sender == (NotificationSender*)m_parentNode)
    {
        if(type == Notification_SceneNodeMoved
        && GetType() == OmniLight)
            m_requireShadowComputation = true;
    }

    SceneObject::Notify(sender, type);
}



int Light::GetMaxNbrLights()
{
    int m = 0;
    glGetIntegerv(GL_MAX_LIGHTS, &m);
    return m;
}


}
