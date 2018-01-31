#ifndef ISOSPRITE_H
#define ISOSPRITE_H

#include <ALAGE/gfx/SpriteEntity.h>

namespace alag
{

class IsometricScene;

class IsoSpriteEntity : public SpriteEntity
{
    friend class IsometricScene;

    public:
        IsoSpriteEntity();
        IsoSpriteEntity(const sf::Vector2i &spriteSize);
        IsoSpriteEntity(const sf::IntRect &textureRect);
        virtual ~IsoSpriteEntity();

        virtual void PrepareShader(sf::Shader *);

        virtual void ComputeShadow(Light*);

        //virtual void Notify(NotificationSender*, NotificationType);

    protected:
        void SetIsoScene(IsometricScene *scene);

    private:
        IsometricScene *m_scene;
};

}



#endif // ISOSPRITE_H
