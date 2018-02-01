#ifndef LIGHT_H
#define LIGHT_H

#include "ALAGE/Types.h"
#include "ALAGE/gfx/SceneObject.h"

namespace alag
{

class ShadowCaster;

class Light : public SceneObject
{
    friend class SceneManager;

    public:
        Light();
        virtual ~Light();

        LightType GetType();
        sf::Vector3f GetDirection();
        sf::Color GetDiffuseColor();
        sf::Color GetSpecularColor();
        float GetConstantAttenuation();
        float GetLinearAttenuation();
        float GetQuadraticAttenuation();
        bool IsCastShadowEnabled();

        void SetType(LightType);
        void SetDirection(sf::Vector3f);
        void SetDiffuseColor(sf::Color);
        void SetSpecularColor(sf::Color);
        void SetConstantAttenuation(float);
        void SetLinearAttunation(float);
        void SetQuadraticAttenuation(float);

        void EnableShadowCasting();
        void DisableShadowCasting();
        const sf::Texture& GetShadowMap();
        const sf::IntRect& GetShadowMaxShift();

        void InitShadowMap();
        void RenderShadowMap(const sf::View &,const sf::Vector2u &);
        void UpdateShadow();

    protected:
        std::list<ShadowCaster*> *GetShadowCasterList();

    private:
        LightType    m_type;
        sf::Vector3f m_direction;
        sf::Color m_diffuseColor;
        sf::Color m_specularColor;
        float m_constantAttenuation;
        float m_linearAttenuation;
        float m_quadraticAttenuation;

        bool m_castShadow;
        sf::RenderTexture m_shadowMap;
        sf::IntRect m_shadowMaxShift;
        bool m_requireShadowComputation;

        std::list<ShadowCaster*> m_shadowCasterList;

};

}

#endif // LIGHTENTITY_H
