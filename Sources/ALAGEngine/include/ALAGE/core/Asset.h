#ifndef ASSET_H
#define ASSET_H

#include <iostream>

namespace alag
{

class Asset
{
    public:
        Asset();
        virtual ~Asset();

        virtual bool LoadFromFile(const std::string &) = 0;

    protected:

    private:
};


}

#endif // ASSET_H
