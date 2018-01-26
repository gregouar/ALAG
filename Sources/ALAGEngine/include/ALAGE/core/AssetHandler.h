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
        friend class Singleton<AssetHandler<AssetType> >;


        AssetType* GetAsset(const AssetTypeID &assetID);

        AssetType* LoadAssetFromFile(const std::string &, AssetLoadType = LoadTypeNow);
        AssetType* LoadAssetFromMemory(void *data, std::size_t dataSize, AssetLoadType = LoadTypeNow);
        AssetType* LoadAssetFromStream(sf::InputStream *stream, AssetLoadType = LoadTypeNow);

        AssetType* LoadAssetFromFile(const AssetTypeID& id,const std::string &, AssetLoadType = LoadTypeNow);
        AssetType* LoadAssetFromMemory(const AssetTypeID& id, void *data, std::size_t dataSize, AssetLoadType = LoadTypeNow);
        AssetType* LoadAssetFromStream(const AssetTypeID& id,sf::InputStream *stream, AssetLoadType = LoadTypeNow);

        AssetType* AddAsset(const AssetTypeID &assetID, bool plannedObsolescence=false, int lifeSpan=1);

        void AddToLoadingThread(AssetType*);
        void RemoveFromLoadingThread(AssetType*);

        AssetTypeID GenerateID();

        void AddToObsolescenceList(const AssetTypeID &assetID,int lifeSpan = 1);
        void RemoveFromObsolescenceList(const AssetTypeID &assetID);

        void DescreaseObsolescenceLife();
        void DeleteAsset(AssetType* );
        void DeleteAsset(const AssetTypeID &assetID);
        void CleanAll();

        void SetDummyAsset(AssetType);
        AssetType* GetDummyAsset();


    protected:
        AssetHandler();
        virtual ~AssetHandler();

        void LoadInThread();

        void LockLoadMutex();
        void UnlockLoadMutex();
        void WaitForLoadingThread(AssetType *assetToWaitFor);

    private:
        std::map<AssetTypeID, AssetType*> m_assets;
        std::map<AssetTypeID, int> m_obsolescenceList;
        std::map<std::string, AssetTypeID> m_filesList;

        sf::Thread* m_loadThread;
        sf::Mutex m_loadMutex;
        std::list<AssetType*> m_assetsToLoadInThread;
        AssetType* m_assetLoadingInThread;

        int m_curNewId;
        bool m_hasDummyAsset;
        AssetType m_dummyAsset;
};

}


#include "../src/core/AssetHandler.inc"


#endif // ASSETHANDLER_H
