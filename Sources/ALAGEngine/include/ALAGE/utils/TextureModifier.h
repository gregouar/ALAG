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
        /*static const sf::Texture* GeneratePerlinNoise(sf::Vector2u gridSize, float rotation,
                                                float , unsigned int levelOfFractal,
                                                bool levelOfSeamless, int seed = 0);*/
        static const sf::Texture& GeneratePerlinNoise(sf::Vector2f *noiseGradients, sf::Vector2u gridSize,
                                                sf::Vector2u textureSize,
                                                float freq = 1.0, float freqMult = 2.0, float amp = 1.0, float ampMult = 0.5,
                                                unsigned int levelOfFractal = 5, bool useAbs = false);

        //static const sf::Texture* GetDerivativePerlinNoise();

    protected:
        TextureModifier();
        virtual ~TextureModifier();

    private:
        sf::Shader m_blurShader;
        sf::Shader m_perlinNoiseShader;

        sf::Vector2u lastGridSize, lastTextureSize;
        sf::RenderTexture m_perlinNoise;


        void CompilePerlinNoiseShader();
};


}

#endif // TEXTUREMANIPULATOR_H
