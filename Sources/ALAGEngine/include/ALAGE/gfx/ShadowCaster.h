#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

#include <SFML/Graphics.hpp>

#include "ALAGE/gfx/Light.h"

namespace alag
{

class ShadowCaster : public SceneObject
{
    public:
        ShadowCaster();
        virtual ~ShadowCaster();

        virtual void ComputeShadow(Light*) = 0; /**Do not forget to AddToLightList()**/
        virtual void RenderShadow(sf::RenderTarget*, Light*);

        sf::IntRect GetShadowMaxShift(Light* light);

        bool IsRequiringShadowCasting(Light*);
        ShadowCastingType GetShadowCastingType();

    protected:
        void AddToLightList(Light*);
        void ForceShadowCastingType(ShadowCastingType);
        void AskForDynamicShadowUpdate();
        void AskForAllShadowUpdate();

        std::map<Light*,sf::Texture> m_shadowMap;
        std::map<Light*,sf::Sprite> m_shadowSprite;
        std::map<Light*,sf::IntRect> m_shadowMaxShift;

    private:
        ShadowCastingType m_shadowCastingType;
        std::map<Light*, bool> m_requireShadowCasting;

};

}

#endif // SPRITEENTITY_H
