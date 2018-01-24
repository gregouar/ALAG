#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneObject.h"

namespace alag
{

class Camera : public SceneObject
{
    public:
        Camera();
        virtual ~Camera();

        float GetZoom();
        sf::Vector2f GetSize();

        void Zoom(float);

        void SetZoom(float );
        void SetSize(sf::Vector2f );

    protected:

    private:
        sf::Vector2f m_size;
        float m_zoom;
};

}

#endif // CAMERA_H
