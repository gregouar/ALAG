#ifndef ASSET_H
#define ASSET_H

#include "ALAGE/core/NotificationSender.h"
#include "ALAGE/Types.h"

#include <iostream>
#include <SFML/System.hpp>

namespace alag
{

class Asset : public NotificationSender
{
    public:
        Asset();
        Asset(const AssetTypeID &);
        virtual ~Asset();

        virtual bool LoadFromFile(const std::string &, AssetLoadType = LoadTypeNow);
        virtual bool LoadFromMemory(void *data, std::size_t size, AssetLoadType = LoadTypeNow);
        virtual bool LoadFromStream(sf::InputStream *stream, AssetLoadType = LoadTypeNow);

        virtual bool LoadNow();

        bool IsLoaded();
        const std::string& GetFilePath();

        const AssetTypeID &GetID();
        const AssetLoadType GetLoadType();
        const AssetLoadSource GetLoadSource();

        void ForceLoadType(AssetLoadType);

    protected:

        bool m_allowLoadFromFile;
        std::string m_filePath;
        std::string m_fileDirectory;

        bool m_allowLoadFromMemory;
        void *m_loadData;
        std::size_t m_loadDataSize;

        bool m_allowLoadFromStream;
        sf::InputStream *m_loadStream;

        AssetLoadSource m_loadSource;
        AssetLoadType m_loadType;

        bool m_loaded;

        std::string m_name;

    private:
        AssetTypeID m_id;

};


}

#endif // ASSET_H
