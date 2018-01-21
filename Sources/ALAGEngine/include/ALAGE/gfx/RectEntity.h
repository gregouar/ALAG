#ifndef RECTENTITY_H
#define RECTENTITY_H

#include <SFML/Graphics.hpp>
#include "ALAGE/gfx/SceneEntity.h"

namespace alag
{

class RectEntity : public SceneEntity, public sf::FloatRect
{
    public:
        RectEntity();
        RectEntity(sf::FloatRect rect);
        virtual ~RectEntity();

    protected:

    private:
};

}

#endif // RECTENTITY_H
