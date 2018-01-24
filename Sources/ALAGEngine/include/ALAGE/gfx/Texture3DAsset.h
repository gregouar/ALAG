#ifndef TEXTURE3DASSET_H
#define TEXTURE3DASSET_H

#include "ALAGE/core/NotificationListener.h"
#include "ALAGE/gfx/TextureAsset.h"
#include "extlibs/tinyxml.h"

namespace alag
{

class Texture3DAsset : public TextureAsset, public NotificationListener
{
    public:
        Texture3DAsset();
        Texture3DAsset(const AssetTypeID&);
        virtual ~Texture3DAsset();

        virtual bool LoadNow();


        sf::Texture* GetTexture();
        sf::Texture* GetColorMap();
        sf::Texture* GetNormalMap();
        sf::Texture* GetDepthMap();
        float GetHeight();

        virtual void Notify(NotificationSender* , NotificationType);

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
