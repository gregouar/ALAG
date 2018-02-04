
#include "ALAGE/utils/Logger.h"

namespace alag
{

template<typename AssetType>
AssetHandler<AssetType>::AssetHandler()
{
    m_loadThread = new sf::Thread(&AssetHandler<AssetType>::LoadInThread, this);
    m_curNewId = 0;
    m_assetLoadingInThread = nullptr;
}


template<typename AssetType>
AssetHandler<AssetType>::~AssetHandler()
{
    CleanAll();
    //m_loadThread->terminate();
    delete m_loadThread;
}



template<typename AssetType>
AssetType* AssetHandler<AssetType>::GetAsset(const AssetTypeID& assetID)
{
    typename std::map<AssetTypeID, AssetType*>::iterator assetIter;

    assetIter = m_assets.find(assetID);

    if(assetIter == m_assets.end())
    {
        Logger::Error("Cannot find asset");
        return (nullptr);
    }

    return assetIter->second;
}


template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromFile(
                                        const std::string &filePath, AssetLoadType loadType)
{
    return LoadAssetFromFile(GenerateID(), filePath, loadType);
}

template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromFile(const AssetTypeID& id,
                                        const std::string &filePath, AssetLoadType loadType)
{
	std::map<std::string, AssetTypeID>::iterator filesIter;
	filesIter = m_filesList.find(filePath);
	if(filesIter != m_filesList.end())
	{
        AssetType* foundedAsset = GetAsset(filesIter->second);
        if(loadType == LoadTypeNow && foundedAsset != nullptr)
        {
            foundedAsset->ForceLoadType(loadType);

            RemoveFromLoadingThread(foundedAsset);
            WaitForLoadingThread(foundedAsset);

            foundedAsset->LoadNow();
        }
		return foundedAsset;
    }

    AssetType* newAsset = AddAsset(id);
    newAsset->LoadFromFile(filePath, loadType);
    if(loadType == LoadTypeNow)
        newAsset->LoadNow();
    else
        AddToLoadingThread(newAsset);

	m_filesList[filePath] = id;

    return newAsset;
}


template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromMemory(
                                        void *data, std::size_t dataSize, AssetLoadType loadType)
{
    return LoadAssetFromMemory(GenerateID(), data, dataSize, loadType);
}

template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromMemory(const AssetTypeID& id,
                                        void *data, std::size_t dataSize, AssetLoadType loadType)
{
    Asset* newAsset = AddAsset(id);
    newAsset->LoadFromMemory(data, dataSize, loadType);
    if(loadType == LoadTypeNow)
        newAsset->LoadNow();
    else
        AddToLoadingThread(newAsset);

    return newAsset;
}


template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromStream(
                                        sf::InputStream *stream, AssetLoadType loadType)
{
    return LoadAssetFromStream(GenerateID(), stream, loadType);
}

template<typename AssetType>
AssetType* AssetHandler<AssetType>::LoadAssetFromStream(const AssetTypeID& id,
                                        sf::InputStream *stream, AssetLoadType loadType)
{
    Asset* newAsset = AddAsset(id);
    newAsset->LoadFromStream(stream, loadType);
    if(loadType == LoadTypeNow)
        newAsset->LoadNow();
    else
        AddToLoadingThread(newAsset);

    return newAsset;
}


template<typename AssetType>
AssetType* AssetHandler<AssetType>::AddAsset(const AssetTypeID &assetID, bool plannedObsolescence, int lifeSpan)
{
    sf::Lock lockLoadMutex(m_loadMutex);

    AssetType* newAsset = nullptr;
    typename std::map<AssetTypeID, AssetType*>::iterator assetIter;

    assetIter = m_assets.find(assetID);

    if(assetIter == m_assets.end())
    {
        newAsset = new AssetType(assetID);
        m_assets[assetID] = newAsset;
    } else {
        if(!plannedObsolescence)
            RemoveFromObsolescenceList(assetID);
        newAsset = assetIter->second;
    }

    if(plannedObsolescence)
        AddToObsolescenceList(assetID, lifeSpan);

    return newAsset;
}

template<class AssetType>
AssetTypeID AssetHandler<AssetType>::GenerateID()
{
    return m_curNewId++;
}



template<typename AssetType>
void AssetHandler<AssetType>::AddToLoadingThread(AssetType* asset)
{
    sf::Lock lockLoadMutex(m_loadMutex);

    if(asset != nullptr && !asset->IsLoaded())
    {

        m_assetsToLoadInThread.push_back(asset);
        if(m_assetsToLoadInThread.size() == 1)
            m_loadThread->launch();
    }
}

template<typename AssetType>
void AssetHandler<AssetType>::RemoveFromLoadingThread(AssetType* asset)
{
    sf::Lock lockLoadMutex(m_loadMutex);

    if(asset != nullptr && asset != m_assetLoadingInThread)
    {
        typename std::list<AssetType*>::iterator loadingListIt;
        loadingListIt = m_assetsToLoadInThread.begin();
        while(loadingListIt != m_assetsToLoadInThread.end())
        {
            if(*loadingListIt == asset)
            {
                m_assetsToLoadInThread.erase(loadingListIt);
                loadingListIt = m_assetsToLoadInThread.end();
            } else
                ++loadingListIt;
        }
    }
}

template<typename AssetType>
void AssetHandler<AssetType>:: AddToObsolescenceList(const AssetTypeID &assetID,int lifeSpan)
{
    if(GetAsset(assetID) != nullptr)
        m_obsolescenceList[assetID] = lifeSpan;
}

template<typename AssetType>
void AssetHandler<AssetType>:: RemoveFromObsolescenceList(const AssetTypeID &assetID)
{
    m_obsolescenceList.erase(assetID);
}

template<typename AssetType>
void AssetHandler<AssetType>:: DescreaseObsolescenceLife()
{
    std::map<AssetTypeID, int>::iterator iter;
    for(iter = m_obsolescenceList.begin() ; iter != m_obsolescenceList.end() ; ++iter)
    {
        iter->second--;
        if(iter->second-- <= 0)
        {
            DeleteAsset(iter->first);
            m_obsolescenceList.erase(iter);
        }
    }

}

template<typename AssetType>
void AssetHandler<AssetType>::DeleteAsset(const AssetTypeID &assetID)
{
    sf::Lock lockLoadMutex(m_loadMutex);

    typename std::map<AssetTypeID, AssetType*>::iterator iter;
    iter = m_assets.find(assetID);

    if(iter != m_assets.end())
    {
        if(iter->second != nullptr)
        {
            WaitForLoadingThread(iter->second);

            if(!iter->second->GetFilePath().empty())
                m_filesList.erase(m_filesList.find(iter->second->GetFilePath()));

            if(!iter->second->IsLoaded() && iter->second->GetLoadType() == LoadTypeInThread)
                RemoveFromLoadingThread(iter->second);

            delete iter->second;
            m_assets.erase(iter);
        }
    } else
        Logger::Error("Trying to delete unfindable asset");
}


template<typename AssetType>
void AssetHandler<AssetType>::DeleteAsset(AssetType *asset)
{
    if(asset != nullptr)
        DeleteAsset(asset->GetID());
}


template<typename AssetType>
void AssetHandler<AssetType>::CleanAll()
{
    LockLoadMutex();

    WaitForLoadingThread(m_assetLoadingInThread);

    typename std::map<AssetTypeID, AssetType*>::iterator assetIter;
    for(assetIter = m_assets.begin() ; assetIter != m_assets.end() ; ++assetIter)
        delete assetIter->second;

    m_assets.clear();
    m_obsolescenceList.clear();
    m_filesList.clear();
    m_assetsToLoadInThread.clear();

    UnlockLoadMutex();

    m_loadThread->wait();
}

template<typename AssetType>
AssetType* AssetHandler<AssetType>::GetDummyAsset()
{
    if(!m_hasDummyAsset)
        return nullptr;
    return &m_dummyAsset;
}


template<typename AssetType>
void AssetHandler<AssetType>::SetDummyAsset(AssetType dummy)
{
    m_dummyAsset = dummy;
    m_hasDummyAsset = true;
}


template<typename AssetType>
void AssetHandler<AssetType>::LoadInThread()
{
    LockLoadMutex();
    while(!m_assetsToLoadInThread.empty())
    {
        if(m_assetsToLoadInThread.front() != nullptr)
        {
            if(!m_assetsToLoadInThread.front()->IsLoaded())
            {
                AssetType* curAsset = m_assetsToLoadInThread.front();

                m_assetLoadingInThread = curAsset;

                m_loadingCurAssetMutex.lock();
                UnlockLoadMutex();
                curAsset->LoadNow();
                m_assetLoadingInThread = nullptr;
                m_loadingCurAssetMutex.unlock();
                LockLoadMutex();
            }
            m_assetsToLoadInThread.erase(m_assetsToLoadInThread.begin());
        } else
            Logger::Error("Tried to load null asset in thread");
    }
    UnlockLoadMutex();
}


template<typename AssetType>
void AssetHandler<AssetType>::LockLoadMutex()
{
    m_loadMutex.lock();
}

template<typename AssetType>
void AssetHandler<AssetType>::UnlockLoadMutex()
{
    m_loadMutex.unlock();
}


template<typename AssetType>
void AssetHandler<AssetType>::WaitForLoadingThread(AssetType *asset)
{
    //if(asset != nullptr)
    //while(asset == m_assetLoadingInThread){std::cout<<m_assetLoadingInThread<<std::endl;};
    if(asset != nullptr && asset == m_assetLoadingInThread)
    {
        m_loadingCurAssetMutex.lock();
        m_loadingCurAssetMutex.unlock();
    }
}


}
