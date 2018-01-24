#include "ALAGE/gfx/Camera.h"

namespace alag
{

Camera::Camera()
{
    m_zoom = 1.0;
    m_size = sf::Vector2f(1,1);
}

Camera::~Camera()
{
    //dtor
}


float Camera::GetZoom()
{
    return m_zoom;
}

sf::Vector2f Camera::GetSize()
{
    return m_size;
}

void Camera::Zoom(float zoom)
{
    if(zoom > 0)
        m_zoom *= zoom;
}

void Camera::SetZoom(float zoom)
{
    if(zoom > 0)
        m_zoom = zoom;
}

void Camera::SetSize(sf::Vector2f newSize)
{
    if(newSize.x != 0 && newSize.y != 0)
        m_size = newSize;
}

}
