#ifndef ISORECT_H
#define ISORECT_H

#include <ALAGE/gfx/RectEntity.h>

namespace alag
{

class IsoRectEntity : public RectEntity
{
    public:
        IsoRectEntity();
        IsoRectEntity(sf::Vector2f rectSize);
        virtual ~IsoRectEntity();

        virtual void PrepareShader(sf::Shader *);

    protected:

    private:
};

}

#endif // ISORECT_H
