#ifndef TEXTURE3DASSET_H
#define TEXTURE3DASSET_H

#include "ALAGE/gfx/TextureAsset.h"
#include "extlibs/tinyxml.h"

namespace alag
{

class Texture3DAsset : public TextureAsset, public LoadedAssetListener
{
    public:
        Texture3DAsset();
        Texture3DAsset(const AssetTypeID&);
        virtual ~Texture3DAsset();

        virtual bool LoadNow();


        sf::Texture* GetTexture(LoadedAssetListener*  = nullptr);
        sf::Texture* GetColorMap(LoadedAssetListener*  = nullptr);
        sf::Texture* GetNormalMap(LoadedAssetListener*  = nullptr);
        sf::Texture* GetDepthMap(LoadedAssetListener*  = nullptr);
        float GetHeight();

        virtual void NotifyLoadedAsset(Asset*);

    protected:
        bool LoadFromXML(TiXmlHandle *);

    private:
        //sf::Texture m_normalMap;
       // sf::Texture m_heightMap;
        TextureAsset *m_colorMap;
        TextureAsset *m_normalMap;
        TextureAsset *m_depthMap;

        float m_default_height;
};

}

#endif // 3DTEXTUREASSET_H
