#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/Texture3DAsset.h"
#include "ALAGE/utils/Parser.h"
#include "ALAGE/utils/Logger.h"
#include <istream>

namespace alag
{

Texture3DAsset::Texture3DAsset()
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_allowLoadFromStream = false;

    m_default_height = 0;

    m_colorMap  = nullptr;
    m_normalMap = nullptr;
    m_depthMap = nullptr;
}

Texture3DAsset::Texture3DAsset(const AssetTypeID& id) : TextureAsset(id)
{
    m_default_height = 0;

    m_colorMap  = nullptr;
    m_normalMap = nullptr;
    m_depthMap = nullptr;
}

Texture3DAsset::~Texture3DAsset()
{
    //dtor
}


bool Texture3DAsset::LoadNow()
{
    bool loaded = true;

    if(!m_loaded)
    {
        if(m_loadSource == LoadSourceFile)
        {
            TiXmlDocument file(m_filePath.c_str());

            if(!file.LoadFile())
            {
                Logger::Error("Cannot load texture3D from file: "+m_filePath);
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
                Logger::Write("Texture3D readed from file: "+m_filePath);

        } else {
            Logger::Error("Cannot load asset");
            loaded = false;
        }

        if(m_loadType == LoadTypeNow)
            m_loaded = loaded;
    }

    //Asset::LoadNow();
    return loaded;
}

bool Texture3DAsset::LoadFromXML(TiXmlHandle *hdl)
{
    if(hdl == nullptr) return (false);

    if(hdl->FirstChildElement("name").Element() != nullptr)
        m_name = hdl->FirstChildElement("name").Element()->GetText();

    if(hdl->FirstChildElement("height").Element() != nullptr)
        m_default_height = Parser::ParseFloat(hdl->FirstChildElement("height").Element()->GetText());

    TiXmlElement* textElem = hdl->FirstChildElement("texture").Element();
    while(textElem != nullptr)
    {
        if(std::string(textElem->Attribute("type")).compare("color") == 0)
        {
            m_colorMap = AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            m_colorMap->AskForAllNotifications(this);
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
        textElem = textElem->NextSiblingElement("texture");
    }

    return (true);
}


sf::Texture* Texture3DAsset::GetTexture()
{
    return GetColorMap();
}

sf::Texture* Texture3DAsset::GetColorMap()
{
    if(m_loaded && m_colorMap != nullptr)
        return m_colorMap->GetTexture();

    return (nullptr);
}

sf::Texture* Texture3DAsset::GetNormalMap()
{
    if(m_loaded && m_normalMap != nullptr)
        return m_normalMap->GetTexture();

    return (nullptr);
}

sf::Texture* Texture3DAsset::GetDepthMap()
{
    if(m_loaded && m_depthMap != nullptr)
        return m_depthMap->GetTexture();

    return (nullptr);
}

float Texture3DAsset::GetHeight()
{
    return m_default_height;
}


void Texture3DAsset::Notify(NotificationSender* sender, NotificationType notification)
{

    if(notification == AssetLoadedNotification)
    if(sender == m_colorMap || sender == m_depthMap || sender == m_normalMap)
    {
        if(m_colorMap != nullptr && m_colorMap->IsLoaded())
        if(m_depthMap != nullptr && m_depthMap->IsLoaded())
        if(m_normalMap != nullptr && m_normalMap->IsLoaded())
        {
            m_loaded = true, Asset::LoadNow();
            Logger::Write("Texture3D loaded from file: "+m_filePath);
        }
    }

    if(notification == NotificationSenderDestroyed)
    {
        if(sender == m_colorMap)
            m_colorMap = nullptr;
        else if(sender == m_depthMap)
            m_depthMap = nullptr;
        else if(sender == m_normalMap)
            m_normalMap = nullptr;
    }
}

/*void Texture3DAsset::NotifyLoadedAsset(Asset* asset)
{

    if(m_colorMap != nullptr && m_colorMap->IsLoaded())
    if(m_depthMap != nullptr && m_depthMap->IsLoaded())
    if(m_normalMap != nullptr && m_normalMap->IsLoaded())
        m_loaded = true, Asset::LoadNow();
}*/

}
