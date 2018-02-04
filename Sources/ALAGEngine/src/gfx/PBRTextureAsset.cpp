#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/PBRTextureAsset.h"
#include "ALAGE/utils/Parser.h"
#include "ALAGE/utils/Logger.h"
#include <istream>

namespace alag
{

PBRTextureAsset::PBRTextureAsset()
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_allowLoadFromStream = false;

    m_default_height = 0;
    m_roughness = 1;
    m_metalness = 0;
    m_translucency = 0;

    m_albedoMap  = nullptr;
    m_normalMap = nullptr;
    m_depthMap = nullptr;
    m_materialMap = nullptr;
}

PBRTextureAsset::PBRTextureAsset(const AssetTypeID& id) : TextureAsset(id)
{
    m_default_height = 0;
    m_roughness = 1;
    m_metalness = 0;
    m_translucency = 0;

    m_albedoMap  = nullptr;
    m_normalMap = nullptr;
    m_depthMap = nullptr;
    m_materialMap = nullptr;
}

PBRTextureAsset::~PBRTextureAsset()
{
    //dtor
}


bool PBRTextureAsset::LoadNow()
{
    bool loaded = true;

    if(!m_loaded)
    {
        if(m_loadSource == LoadSourceFile)
        {
            TiXmlDocument file(m_filePath.c_str());

            if(!file.LoadFile())
            {
                Logger::Error("Cannot load PBR texture from file: "+m_filePath);
                std::ostringstream errorReport;
                errorReport << "Because: "<<file.ErrorDesc();
                Logger::Error(errorReport);
                loaded = false;
            } else {
                TiXmlHandle hdl(&file);
                hdl = hdl.FirstChildElement();
                loaded = LoadFromXML(&hdl);
            }

            if(loaded)
                Logger::Write("PBRTexture readed from file: "+m_filePath);

        } else {
            Logger::Error("Cannot load PBR texture from other source than file");
            loaded = false;
        }

        if(m_loadType == LoadTypeNow)
            m_loaded = loaded;
    }

    //Asset::LoadNow();
    return loaded;
}

bool PBRTextureAsset::LoadFromXML(TiXmlHandle *hdl)
{
    if(hdl == nullptr) return (false);

    if(hdl->FirstChildElement("name").Element() != nullptr)
        m_name = hdl->FirstChildElement("name").Element()->GetText();

    if(hdl->FirstChildElement("height").Element() != nullptr)
        m_default_height = Parser::ParseFloat(hdl->FirstChildElement("height").Element()->GetText());

    TiXmlElement* textElem = hdl->FirstChildElement("material").Element();
    while(textElem != nullptr)
    {
        if(std::string(textElem->Attribute("type")).compare("roughness") == 0)
        {
            float r = Parser::ParseFloat(textElem->GetText());
            if(r >= 0 && r <= 1)
                m_roughness = r;
            else
                Logger::Error("Cannot read roughness in PBR texture"+m_filePath);
        } else if(std::string(textElem->Attribute("type")).compare("metalness") == 0)
        {
            float r = Parser::ParseFloat(textElem->GetText());
            if(r >= 0 && r <= 1)
                m_metalness = r;
            else
                Logger::Error("Cannot read metalness in PBR texture"+m_filePath);
        } else if(std::string(textElem->Attribute("type")).compare("translucency") == 0)
        {
            float r = Parser::ParseFloat(textElem->GetText());
            if(r >= 0 && r <= 1)
                m_translucency = r;
            else
                Logger::Error("Cannot read translucency in PBR texture"+m_filePath);
        }
        textElem = textElem->NextSiblingElement("material");
    }

    textElem = hdl->FirstChildElement("texture").Element();
    while(textElem != nullptr)
    {
        if(std::string(textElem->Attribute("type")).compare("albedo") == 0)
        {
            m_albedoMap = AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            m_albedoMap->AskForAllNotifications(this);
        }
        else if(std::string(textElem->Attribute("type")).compare("normal") == 0)
        {
            m_normalMap = AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            m_normalMap->AskForAllNotifications(this);

        }
        else if(std::string(textElem->Attribute("type")).compare("depth") == 0)
        {
            m_depthMap = AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            m_depthMap->AskForAllNotifications(this);
        }
        else if(std::string(textElem->Attribute("type")).compare("material") == 0)
        {
            m_materialMap = AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            m_materialMap->AskForAllNotifications(this);
        }
        textElem = textElem->NextSiblingElement("texture");
    }

    return (true);
}

void PBRTextureAsset::PrepareShader(sf::Shader *shader)
{
    if(GetAlbedoMap() != nullptr)
        shader->setUniform("map_albedo",*GetAlbedoMap());

    if(GetDepthMap() != nullptr)
    {
        shader->setUniform("map_depth",*GetDepthMap());
        shader->setUniform("enable_depthMap", true);
    } else {
        shader->setUniform("enable_depthMap", false);
    }

    if(GetNormalMap() != nullptr)
    {
        shader->setUniform("map_normal",*GetNormalMap());
        shader->setUniform("enable_normalMap", true);
    } else {
        shader->setUniform("enable_normalMap", false);
    }

    if(GetMaterialMap() != nullptr)
    {
        shader->setUniform("map_material",*GetNormalMap());
        shader->setUniform("enable_materialMap", true);
    } else {
        shader->setUniform("enable_materialMap", false);
    }

    shader->setUniform("p_height",GetHeight());
    shader->setUniform("p_roughness",GetRoughness());
    shader->setUniform("p_metalness",GetMetalness());
    shader->setUniform("p_translucency",GetTranslucency());
}


sf::Texture* PBRTextureAsset::GetTexture()
{
    return GetAlbedoMap();
}

sf::Texture* PBRTextureAsset::GetAlbedoMap()
{
    if(m_loaded && m_albedoMap != nullptr)
        return m_albedoMap->GetTexture();

    return (nullptr);
}

sf::Texture* PBRTextureAsset::GetNormalMap()
{
    if(m_loaded && m_normalMap != nullptr)
        return m_normalMap->GetTexture();

    return (nullptr);
}

sf::Texture* PBRTextureAsset::GetDepthMap()
{
    if(m_loaded && m_depthMap != nullptr)
        return m_depthMap->GetTexture();

    return (nullptr);
}

sf::Texture* PBRTextureAsset::GetMaterialMap()
{
    if(m_loaded && m_materialMap != nullptr)
        return m_materialMap->GetTexture();

    return (nullptr);
}

float PBRTextureAsset::GetHeight()
{
    return m_default_height;
}

float PBRTextureAsset::GetRoughness()
{
    return m_roughness;
}

float PBRTextureAsset::GetMetalness()
{
    return m_metalness;
}

float PBRTextureAsset::GetTranslucency()
{
    return m_translucency;
}

void PBRTextureAsset::Notify(NotificationSender* sender, NotificationType notification)
{
    if(notification == AssetLoadedNotification)
    if(sender == m_albedoMap || sender == m_depthMap
       || sender == m_normalMap || sender == m_materialMap)
    {
        if(m_albedoMap == nullptr || m_albedoMap->IsLoaded())
        if(m_depthMap == nullptr || m_depthMap->IsLoaded())
        if(m_normalMap == nullptr || m_normalMap->IsLoaded())
        if(m_materialMap == nullptr || m_materialMap->IsLoaded())
        {
            m_loaded = true, Asset::LoadNow();
            Logger::Write("PBR texture loaded from file: "+m_filePath);
        }
    }

    if(notification == NotificationSenderDestroyed)
    {
        if(sender == m_albedoMap)
            m_albedoMap = nullptr;
        else if(sender == m_depthMap)
            m_depthMap = nullptr;
        else if(sender == m_normalMap)
            m_normalMap = nullptr;
        else if(sender == m_materialMap)
            m_materialMap = nullptr;
    }
}

/*void PBRTextureAsset::NotifyLoadedAsset(Asset* asset)
{

    if(m_colorMap != nullptr && m_colorMap->IsLoaded())
    if(m_depthMap != nullptr && m_depthMap->IsLoaded())
    if(m_normalMap != nullptr && m_normalMap->IsLoaded())
        m_loaded = true, Asset::LoadNow();
}*/

}
