#ifndef ISOMETRICSCENEMANAGER_H
#define ISOMETRICSCENEMANAGER_H

#include "ALAGE/gfx/SceneManager.h"
#include <SFML/OpenGL.hpp>

namespace alag
{

class IsometricScene : public SceneManager
{
    public:
        IsometricScene();
        virtual ~IsometricScene();

        virtual bool InitRenderer();
        virtual void RenderScene(sf::RenderTarget*);

    protected:

    private:
};

}

#endif // ISOMETRICSCENEMANAGER_H
