#ifndef ISOSPRITE_H
#define ISOSPRITE_H

#include <ALAGE/gfx/SpriteEntity.h>

namespace alag
{

class IsoSpriteEntity : public SpriteEntity
{
    public:
        IsoSpriteEntity();
        IsoSpriteEntity(const sf::Vector2i &spriteSize);
        IsoSpriteEntity(const sf::IntRect &textureRect);
        virtual ~IsoSpriteEntity();

        virtual void PrepareShader(sf::Shader *);

        void SetIsoToCartZFactor(float );

    protected:

    private:
        float m_isoToCartZFactor;
};

}



#endif // ISOSPRITE_H
