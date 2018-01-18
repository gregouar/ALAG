#include "ALAGE/core/Asset.h"
#include "ALAGE/utils/Parser.h"
#include "ALAGE/utils/Logger.h"

namespace alag
{

Asset::Asset()
{
    m_allowLoadFromFile = false;
    m_allowLoadFromMemory = false;
    m_allowLoadFromStream = false;
    m_loadSource = NoLoadSource;
    m_loaded = false;
}

Asset::~Asset()
{
    //dtor
}

bool Asset::LoadFromFile(const std::string &filePath, AssetLoadType loadType)
{
    if(!m_allowLoadFromFile)
    {
        Logger::Error("Asset is not allowed to load from file");
        return (false);
    }

    m_loadSource = LoadSourceFile;
    m_filePath = filePath;
    m_fileDirectory = Parser::FindFileDirectory(m_filePath);
    m_loadType = loadType;

    return m_loaded;
}

bool Asset::LoadFromMemory(void *data, std::size_t dataSize, AssetLoadType loadType)
{
    if(!m_allowLoadFromMemory)
    {
        Logger::Error("Asset is not allowed to load from memory");
        return (false);
    }

    m_loadSource = LoadSourceMemory;
    m_loadData = data;
    m_loadDataSize = dataSize;
    m_loadType = loadType;

    return m_loaded;
}

bool Asset::LoadFromStream(sf::InputStream *stream, AssetLoadType loadType)
{
    if(!m_allowLoadFromStream)
    {
        Logger::Error("Asset is not allowed to load from stream");
        return (false);
    }

    m_loadSource = LoadSourceStream;
    m_loadStream = stream;
    m_loadType = loadType;

    return m_loaded;
}

bool Asset::IsLoaded()
{
    return (m_loaded);
}

}
