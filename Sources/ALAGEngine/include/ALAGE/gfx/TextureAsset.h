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
        TextureAsset(const AssetTypeID&);
        TextureAsset(const sf::Image &img);
        TextureAsset(sf::Texture *);
        virtual ~TextureAsset();

        virtual bool LoadNow();

        void GenerateMipmap();
        void SetSmooth(bool = true);

        virtual sf::Texture* GetTexture();

    protected:
        sf::Texture* m_texture;

    private:
        bool m_createdTexture;
};

}

#endif // TEXTUREASSET_H
