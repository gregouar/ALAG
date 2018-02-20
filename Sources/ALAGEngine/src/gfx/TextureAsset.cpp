#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

TextureAsset::TextureAsset()
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = true;
    m_allowLoadFromStream = true;

    m_texture.setRepeated(true);
    m_texture.setSmooth(true);
}

TextureAsset::TextureAsset(const AssetTypeID& id) : Asset(id)
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = true;
    m_allowLoadFromStream = true;

    m_texture.setRepeated(true);
    m_texture.setSmooth(true);
}


TextureAsset::TextureAsset(const sf::Image &img) : TextureAsset()
{
    m_texture.loadFromImage(img);
    m_loaded = true;
}


TextureAsset::~TextureAsset()
{
}

bool TextureAsset::LoadNow()
{
    bool loaded = true;

    if(!m_loaded) {
        if(m_loadSource == LoadSource_File)
        {
            if(!m_texture.loadFromFile(m_filePath))
            {
                Logger::Error("Cannot load texture from file: "+m_filePath);
                loaded = false;
            } else
                Logger::Write("Texture loaded from file: "+m_filePath);
        } else if(m_loadSource == LoadSource_Memory) {
            if(!m_texture.loadFromMemory(m_loadData,m_loadDataSize))
            {
                Logger::Error("Cannot load texture from memory");
                loaded = false;
            }
        } else if(m_loadSource == LoadSource_Stream) {
            if(!m_texture.loadFromStream(*m_loadStream))
            {
                Logger::Error("Cannot load texture from stream");
                loaded = false;
            }
        } else {
            Logger::Error("Cannot load asset");
            m_loaded = false;
        }

        m_loaded = loaded;
    }

    return Asset::LoadNow();
}

sf::Texture *TextureAsset::GetTexture()
{
    if(m_loaded)
        return &m_texture;

    if(AssetHandler<TextureAsset>::Instance()->GetDummyAsset() != nullptr)
        return AssetHandler<TextureAsset>::Instance()->GetDummyAsset()->GetTexture();
    else
        return nullptr;
}

}
