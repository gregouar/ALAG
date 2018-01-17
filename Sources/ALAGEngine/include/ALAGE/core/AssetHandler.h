#ifndef ASSETHANDLER_H
#define ASSETHANDLER_H

#include "ALAGE/core/Asset.h"
#include <vector>

namespace alag
{

class AssetHandler
{
    public:
        AssetHandler();
        virtual ~AssetHandler();

        alag::Asset* GetAsset(unsigned int );

    protected:
        std::vector<alag::Asset*> m_assets;

    private:
};

}

#endif // ASSETHANDLER_H
