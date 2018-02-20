#include "ALAGE/gfx/FontAsset.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

FontAsset::FontAsset()
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_allowLoadFromStream = false;
}

FontAsset::FontAsset(const AssetTypeID& id) : Asset(id)
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_allowLoadFromStream = false;
}

FontAsset::~FontAsset()
{
}

bool FontAsset::LoadNow()
{
    bool loaded = true;

    if(!m_loaded) {
        if(m_loadSource == LoadSource_File)
        {
            if(!m_font.loadFromFile(m_filePath))
            {
                Logger::Error("Cannot load font from file: "+m_filePath);
                loaded = false;
            } else
                Logger::Write("Font loaded from file: "+m_filePath);
        } else {
            Logger::Error("Cannot load font from other source than file");
            m_loaded = false;
        }

        m_loaded = loaded;
    }

    return Asset::LoadNow();
}

sf::Font *FontAsset::GetFont()
{
    if(m_loaded)
        return &m_font;

    return nullptr;
}

}
