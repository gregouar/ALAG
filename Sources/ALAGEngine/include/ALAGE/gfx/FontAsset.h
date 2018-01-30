#ifndef FONTASSET_H
#define FONTASSET_H

#include "ALAGE/core/Asset.h"
#include <SFML/Graphics.hpp>


namespace alag
{

class FontAsset : public Asset
{
    public:
        FontAsset();
        FontAsset(const AssetTypeID&);
        virtual ~FontAsset();


        virtual bool LoadNow();

        virtual sf::Font* GetFont();


    protected:
        sf::Font m_font;

    private:
};

}

#endif // FONTASSET_H
