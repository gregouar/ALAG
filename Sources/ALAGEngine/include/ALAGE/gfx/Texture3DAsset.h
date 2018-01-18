#ifndef TEXTURE3DASSET_H
#define TEXTURE3DASSET_H

#include "ALAGE/gfx/TextureAsset.h"
#include "extlibs/tinyxml.h"

namespace alag
{

class Texture3DAsset : public Asset
{
    public:
        Texture3DAsset();
        virtual ~Texture3DAsset();

        virtual bool LoadNow();

        sf::Texture* GetColorMap();
        sf::Texture* GetNormalMap();
        sf::Texture* GetHeightMap();

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
