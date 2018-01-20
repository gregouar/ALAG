#include "ALAGE/gfx/IsometricScene.h"

namespace alag
{

IsometricScene::IsometricScene()
{
    //ctor
}

IsometricScene::~IsometricScene()
{
    //dtor
}


bool IsometricScene::InitRenderer()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
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
