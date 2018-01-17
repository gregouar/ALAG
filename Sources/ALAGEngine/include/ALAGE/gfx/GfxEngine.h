#ifndef GFXENGINE_H
#define GFXENGINE_H

#include "ALAGE/utils/singleton.h"
#include <SFML/Graphics.hpp>

namespace alag
{

class GfxEngine : public Singleton<GfxEngine>
{
    public:
        friend class Singleton<GfxEngine>;

        void RenderWorld(sf::RenderTarget*);
        void SetView(const sf::View&);

    protected:
        GfxEngine();
        virtual ~GfxEngine();

    private:
};

}

#endif // GFXENGINE_H
