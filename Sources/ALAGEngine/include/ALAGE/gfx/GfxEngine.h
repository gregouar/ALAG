#ifndef GFXENGINE_H
#define GFXENGINE_H

#include "ALAGE/utils/singleton.h"

namespace alag
{

class GfxEngine : public Singleton<GfxEngine>
{
    public:
        friend class Singleton<GfxEngine>;

    protected:
        GfxEngine();
        virtual ~GfxEngine();

    private:
};

}

#endif // GFXENGINE_H
