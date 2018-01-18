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
}

Texture3DAsset::~Texture3DAsset()
{
    //dtor
}


bool Texture3DAsset::LoadNow()
{
    m_loaded = true;

    if(m_loadSource == LoadSourceFile)
    {
        TiXmlDocument file(m_filePath.c_str());

        if(!file.LoadFile())
        {
            Logger::Error("Cannot load texture3D from file: "+m_filePath);
            std::ostringstream errorReport;
            errorReport << "Because: "<<file.ErrorDesc();
            Logger::Error(errorReport.str());
            m_loaded = false;
        } else {
            TiXmlHandle hdl(&file);
            hdl = hdl.FirstChildElement();
            m_loaded = LoadFromXML(&hdl);
        }

        if(m_loaded)
            Logger::Write("Texture3D loaded from file: "+m_filePath);

    } else {
        Logger::Error("Cannot load asset");
        m_loaded = false;
    }

    return (m_loaded);
}

bool Texture3DAsset::LoadFromXML(TiXmlHandle *hdl)
{
    if(hdl == NULL) return (false);

    if(hdl->FirstChildElement("name").Element() != NULL)
        m_name = hdl->FirstChildElement("name").Element()->GetText();

    if(hdl->FirstChildElement("height").Element() != NULL)
        m_default_height = Parser::ParseInt(hdl->FirstChildElement("height").Element()->GetText());

    TiXmlElement* textElem = hdl->FirstChildElement("texture").Element();
    while(textElem != NULL)
    {
        if(std::string(textElem->Attribute("type")).compare("color") == 0)
            m_colorMap = (TextureAsset*) AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
        else if(std::string(textElem->Attribute("type")).compare("normal") == 0)
            m_normalMap = (TextureAsset*) AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
        else if(std::string(textElem->Attribute("type")).compare("depth") == 0)
            m_heightMap = (TextureAsset*) AssetHandler<TextureAsset>::Instance()
                            ->LoadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
        textElem = textElem->NextSiblingElement("texture");
    }

    return (true);
}

}
