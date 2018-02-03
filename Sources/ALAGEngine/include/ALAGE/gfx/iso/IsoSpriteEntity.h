#ifndef ISOSPRITE_H
#define ISOSPRITE_H

#include <ALAGE/gfx/SpriteEntity.h>

namespace alag
{

class PBRIsoScene;

class IsoSpriteEntity : public SpriteEntity
{
    friend class PBRIsoScene;

    public:
        IsoSpriteEntity();
        IsoSpriteEntity(const sf::Vector2i &spriteSize);
        IsoSpriteEntity(const sf::IntRect &textureRect);
        virtual ~IsoSpriteEntity();

        virtual void PrepareShader(sf::Shader *);

        virtual void ComputeShadow(Light*);
        void RenderShadow(sf::RenderTarget*, Light*);

        //virtual void Notify(NotificationSender*, NotificationType);

    protected:
        void SetIsoScene(PBRIsoScene *scene);

    private:
        PBRIsoScene *m_scene;
};

}



#endif // ISOSPRITE_H
