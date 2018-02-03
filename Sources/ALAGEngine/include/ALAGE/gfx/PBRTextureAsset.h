#ifndef PBRTEXTUREASSET_H
#define PBRTEXTUREASSET_H

#include "ALAGE/core/NotificationListener.h"
#include "ALAGE/gfx/TextureAsset.h"
#include "extlibs/tinyxml.h"

namespace alag
{

class PBRTextureAsset : public TextureAsset, public NotificationListener
{
    public:
        PBRTextureAsset();
        PBRTextureAsset(const AssetTypeID&);
        virtual ~PBRTextureAsset();

        virtual bool LoadNow();

        void PrepareShader(sf::Shader *);

        sf::Texture* GetTexture(); //Return albedo
        sf::Texture* GetAlbedoMap();
        sf::Texture* GetNormalMap();
        sf::Texture* GetDepthMap();
        sf::Texture* GetMaterialMap();
        float GetHeight();
        float GetRoughness();
        float GetMetalness();
        float GetTranslucency();

        virtual void Notify(NotificationSender* , NotificationType);

    protected:
        bool LoadFromXML(TiXmlHandle *);

    private:
        TextureAsset *m_albedoMap;
        TextureAsset *m_normalMap;
        TextureAsset *m_depthMap;
        TextureAsset *m_materialMap;

        float m_default_height;
        float m_roughness;
        float m_metalness;
        float m_translucency;
};

}

#endif // PBRTEXTUREASSET_H
