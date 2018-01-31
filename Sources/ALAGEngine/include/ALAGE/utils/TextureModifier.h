#ifndef TEXTUREMODIFIER_H
#define TEXTUREMODIFIER_H

#include <SFML/Graphics.hpp>
#include "ALAGE/utils/Singleton.h"

namespace alag
{

class TextureModifier : public Singleton<TextureModifier>
{
    friend class Singleton<TextureModifier>;

    public:

        static void BlurTexture(sf::Texture* texture, float blur);

    protected:
        TextureModifier();
        virtual ~TextureModifier();

    private:
        sf::Shader m_blurShader;
};


}

#endif // TEXTUREMANIPULATOR_H
