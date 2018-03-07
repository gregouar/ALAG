#ifndef ISORECT_H
#define ISORECT_H

#include <ALAGE/gfx/RectEntity.h>

namespace alag
{

class PBRIsoScene;

class IsoRectEntity : public RectEntity
{
    friend class PBRIsoScene;

    public:
        IsoRectEntity();
        IsoRectEntity(sf::Vector2f rectSize);
        virtual ~IsoRectEntity();

        virtual void PrepareShader(sf::Shader *);

        void SetParallax(bool = true);

    protected:
        void SetIsoScene(PBRIsoScene *scene);
        PBRIsoScene* GetIsoScene();

    private:
        PBRIsoScene *m_scene;
        bool m_enableParallax;
};

}

#endif // ISORECT_H
