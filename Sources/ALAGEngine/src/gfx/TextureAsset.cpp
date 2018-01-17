#include "TextureAsset.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

TextureAsset::TextureAsset()
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = true;
    m_allowLoadFromStream = true;
}

TextureAsset::~TextureAsset()
{
    //dtor
}

bool TextureAsset::LoadNow()
{
    m_loaded = true;

    if(m_loadSource == LoadSourceFile)
    {
        if(!m_texture.loadFromFile(m_filePath))
        {
            Logger::Error("Cannot load texture from file: "+m_filePath);
            m_loaded = false;
        } else
            Logger::Write("Texture loaded from file: "+m_filePath);
    } else if(m_loadSource == LoadSourceMemory) {
        if(!m_texture.loadFromMemory(m_loadData,m_loadDataSize))
        {
            Logger::Error("Cannot load texture from memory");
            m_loaded = false;
        }
    } else if(m_loadSource == LoadSourceStream) {
        if(!m_texture.loadFromStream(*m_loadStream))
        {
            Logger::Error("Cannot load texture from stream");
            m_loaded = false;
        }
    } else {
        Logger::Error("Cannot load asset");
        m_loaded = false;
    }

    return (m_loaded);
}

}
