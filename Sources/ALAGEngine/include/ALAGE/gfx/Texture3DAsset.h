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


        sf::Texture* GetTexture(SceneEntity* entityToNotify = nullptr);
        sf::Texture* GetColorMap(SceneEntity* entityToNotify = nullptr);
        sf::Texture* GetNormalMap(SceneEntity* entityToNotify = nullptr);
        sf::Texture* GetDepthMap(SceneEntity* entityToNotify = nullptr);

    protected:
        bool LoadFromXML(TiXmlHandle *);

    private:
        //sf::Texture m_normalMap;
       // sf::Texture m_heightMap;
        TextureAsset *m_colorMap;
        TextureAsset *m_normalMap;
        TextureAsset *m_depthMap;

        int m_default_height;
};

}

#endif // 3DTEXTUREASSET_H
