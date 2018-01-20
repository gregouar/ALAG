#ifndef TEXTURE3DASSET_H
#define TEXTURE3DASSET_H

#include "ALAGE/gfx/TextureAsset.h"
#include "extlibs/tinyxml.h"

namespace alag
{

class Texture3DAsset : public TextureAsset
{
    public:
        Texture3DAsset();
        virtual ~Texture3DAsset();

        virtual bool LoadNow();


        const sf::Texture& GetTexture();
        const sf::Texture& GetColorMap();
        const sf::Texture& GetNormalMap();
        const sf::Texture& GetHeightMap();

    protected:
        bool LoadFromXML(TiXmlHandle *);

    private:
        //sf::Texture m_normalMap;
       // sf::Texture m_heightMap;
        TextureAsset *m_colorMap;
        TextureAsset *m_normalMap;
        TextureAsset *m_heightMap;

        int m_default_height;
};

}

#endif // 3DTEXTUREASSET_H
