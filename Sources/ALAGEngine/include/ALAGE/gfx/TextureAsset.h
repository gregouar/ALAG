#ifndef TEXTUREASSET_H
#define TEXTUREASSET_H

#include "ALAGE/core/Asset.h"
#include <SFML/Graphics.hpp>

namespace alag
{

class TextureAsset : public Asset
{
    public:
        TextureAsset();
        virtual ~TextureAsset();

        bool LoadNow();

        sf::Texture* GetTexture();

    protected:

    private:
        sf::Texture m_texture;
};

}

#endif // TEXTUREASSET_H
