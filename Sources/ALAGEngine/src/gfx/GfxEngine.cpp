#include "ALAGE/gfx/GfxEngine.h"

namespace alag
{

GfxEngine::GfxEngine()
{
    //ctor
}

GfxEngine::~GfxEngine()
{
    //dtor
}

void GfxEngine::InitRenderer()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void GfxEngine::RenderWorld(sf::RenderTarget* w)
{
    if(w != NULL)
    {
        sf::View oldView = w->getView();
        glClearDepth(1.f);
        w->setView(m_view);



        w->setView(oldView);
    }
}


}
