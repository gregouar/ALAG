#ifndef ASSET_H
#define ASSET_H

#include "ALAGE/Types.h"

#include <iostream>
#include <SFML/System.hpp>

namespace alag
{

class Asset
{
    public:
        Asset();
        virtual ~Asset();

        virtual bool LoadFromFile(const std::string &, AssetLoadType = LoadTypeNow);
        virtual bool LoadFromMemory(void *data, std::size_t size, AssetLoadType = LoadTypeNow);
        virtual bool LoadFromStream(sf::InputStream *stream, AssetLoadType = LoadTypeNow);

        virtual bool LoadNow() = 0;

        bool IsLoaded();

    protected:
        bool m_allowLoadFromFile;
        std::string m_filePath;

        bool m_allowLoadFromMemory;
        void *m_loadData;
        std::size_t m_loadDataSize;

        bool m_allowLoadFromStream;
        sf::InputStream *m_loadStream;

        AssetLoadSource m_loadSource;

        bool m_loaded;

    private:
};


}

#endif // ASSET_H
