#ifndef ASSETHANDLER_H
#define ASSETHANDLER_H

#include "ALAGE/Types.h"
#include "ALAGE/core/Asset.h"
#include "ALAGE/utils/Singleton.h"

#include <map>
#include <vector>
#include <SFML/System.hpp>

namespace alag
{

template<class AssetType> class AssetHandler : public Singleton<AssetHandler<AssetType> >
{
    public:
        //friend AssetHandler<AssetType>* Singleton<AssetHandler<AssetType> >::Instance();
        friend class Singleton<AssetHandler<AssetType> >;


        Asset* GetAsset(const AssetTypeID &assetID);

        Asset* LoadAssetFromFile(const AssetTypeID& id,const std::string &, AssetLoadType = LoadTypeNow);
        Asset* LoadAssetFromMemory(const AssetTypeID& id, void *data, std::size_t dataSize, AssetLoadType = LoadTypeNow);
        Asset* LoadAssetFromStream(const AssetTypeID& id,sf::InputStream *stream, AssetLoadType = LoadTypeNow);

        Asset* AddAsset(const AssetTypeID &assetID, bool plannedObsolescence=false, int lifeSpan=1);
        void AddToLoadingThread(Asset*);

        AssetTypeID GenerateID();

        void AddToObsolescenceList(const AssetTypeID &assetID,int lifeSpan = 1);
        void RemoveFromObsolescenceList(const AssetTypeID &assetID);

        void DescreaseObsolescenceLife();
        void DeleteAsset(const AssetTypeID &assetID);
        void CleanAll();

    protected:
        AssetHandler();
        virtual ~AssetHandler();

        void LoadInThread();

    private:
        std::map<AssetTypeID, Asset*> m_assets;
        std::map<AssetTypeID, int> m_obsolescenceList;

        sf::Thread* m_loadThread;
        std::vector<Asset*> m_assetsToLoadInThread;

        int m_curNewId;
};

}

#endif // ASSETHANDLER_H
