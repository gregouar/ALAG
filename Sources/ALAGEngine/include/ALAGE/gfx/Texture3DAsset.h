#ifndef TEXTURE3DASSET_H
#define TEXTURE3DASSET_H

#include "ALAGE/gfx/TextureAsset.h"

namespace alag
{

class Texture3DAsset : TextureAsset
{
    public:
        Texture3DAsset();
        virtual ~Texture3DAsset();

        sf::Texture* GetNormalMap();
        sf::Texture* GetHeightMap();

    protected:

    private:
        sf::Texture m_normalMap;
        sf::Texture m_heightMap;
};

}

#endif // 3DTEXTUREASSET_H
