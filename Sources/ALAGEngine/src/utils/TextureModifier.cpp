#include "ALAGE/utils/TextureModifier.h"

#include <SFML/Graphics.hpp>
#include <sstream>

namespace alag
{


/*const std::string blur_fragShader = \
    "uniform sampler2D texture;" \
    "uniform vec2 offset;" \
    "void main()" \
    "{" \
    "    gl_FragColor =  gl_Color * "
	"			   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.06 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.75)	* 0.09 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.5)	* 0.12 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.25)	* 0.15 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 0.16 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 1.0) 	* 0.06 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.75)	* 0.09 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.5)	* 0.12 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.25)	* 0.15 ); "
    "}";*/

const std::string blur_fragShader = \
    "uniform sampler2D map_texture;" \
    "uniform vec2 p_offset;" \
    "void main()" \
    "{" \
    "    gl_FragColor =  gl_Color * "
	"			   (texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 1.0)	* 0.000003 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.8)	* 0.000229 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.6)	* 0.005977 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.4)	* 0.060598 + "
	"    			texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.2)	* 0.24173 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy)	* 0.382925 + "
	"			    texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 1.0)	* 0.000003 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.8)	* 0.000229 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.6)	* 0.005977 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.4)	* 0.060598 + "
	"    			texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.2)	* 0.24173); "
    "}";


TextureModifier::TextureModifier()
{
    m_blurShader.loadFromMemory(blur_fragShader,sf::Shader::Fragment);
    m_blurShader.setUniform("map_texture", sf::Shader::CurrentTexture);

    lastGridSize = sf::Vector2u(0,0);
}

TextureModifier::~TextureModifier()
{
    //dtor
}



void TextureModifier::BlurTexture(sf::Texture* texture, float blur)
{
    sf::RectangleShape rect;
    rect.setTexture(texture);
    rect.setSize(sf::Vector2f(texture->getSize().x,
                              texture->getSize().y));

    sf::RenderTexture renderer;
    renderer.create(texture->getSize().x, texture->getSize().y);

    sf::Shader *blurShader = &Instance()->m_blurShader;

    blurShader->setUniform("p_offset",sf::Vector2f(blur/texture->getSize().x,0));
    renderer.draw(rect,blurShader);
    renderer.display();

    rect.setTexture(&renderer.getTexture());
    blurShader->setUniform("p_offset",sf::Vector2f(0,blur/texture->getSize().y));
    renderer.draw(rect,blurShader);
    renderer.display();

    texture->update(renderer.getTexture().copyToImage());
}

const sf::Texture& TextureModifier::GeneratePerlinNoise(sf::Vector2f *noiseGradients, sf::Vector2u gridSize,
                                        sf::Vector2u textureSize,
                                        float freq, float freqMult, float amp, float ampMult,
                                        unsigned int levelOfFractal, bool useAbs)
{
    if(Instance()->lastGridSize != gridSize && gridSize != sf::Vector2u(0,0))
    {
        Instance()->lastGridSize = gridSize;
        Instance()->CompilePerlinNoiseShader();
    }

    Instance()->m_perlinNoiseShader.setUniform("p_frequency", freq);
    Instance()->m_perlinNoiseShader.setUniform("p_frequencyMultiplier", freqMult);
    Instance()->m_perlinNoiseShader.setUniform("p_amplitude", amp);
    Instance()->m_perlinNoiseShader.setUniform("p_amplitudeMultiplier", ampMult);
    Instance()->m_perlinNoiseShader.setUniform("p_fractal", (int)levelOfFractal);
    Instance()->m_perlinNoiseShader.setUniform("p_useAbs", useAbs);
    Instance()->m_perlinNoiseShader.setUniformArray("random_gradients", noiseGradients, gridSize.x*gridSize.y);

    if(textureSize != Instance()->m_perlinNoise.getSize())
    {
        Instance()->m_perlinNoise.create(textureSize.x, textureSize.y, false, true); //We use float to get derivatives
        Instance()->m_perlinNoise.setRepeated(true);
        Instance()->m_perlinNoise.setSmooth(true);
        Instance()->m_perlinNoise.generateMipmap();
    }



    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(textureSize));
    Instance()->m_perlinNoise.draw(rect,&Instance()->m_perlinNoiseShader);
    Instance()->m_perlinNoise.display();

    return Instance()->m_perlinNoise.getTexture();
}


void TextureModifier::CompilePerlinNoiseShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec2 Coord0; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    Coord0 = 0.5+0.5*(gl_ModelViewProjectionMatrix * gl_Vertex); "\
    "    gl_FrontColor = gl_Color; "\
    "}";


    fragShader<<
    "uniform float p_frequency;" \
    "uniform float p_frequencyMultiplier;" \
    "uniform float p_amplitude;" \
    "uniform float p_amplitudeMultiplier;" \
    "uniform int p_fractal;" \
    "uniform bool p_useAbs;" \
    "uniform vec2 random_gradients["<<lastGridSize.x*lastGridSize.y<<"];"
    ""
    "varying vec2 Coord0; "\
    ""
    "const vec3 constantList = vec3(1.0, 0.0, -1.0);"
    ""
    "int GridPos(int x, int y)"
    "{"
    "   x = mod(x,"<<lastGridSize.x<<");"
    "   y = mod(y,"<<lastGridSize.y<<");"
    "   return x + y * "<<lastGridSize.x<<";"
    "}"
    ""
    "float Quintic(float t)"
    "{"
    "   return t*t*t*(t*(6.0*t-15.0)+10.0);"
    "}"
    ""
    "float DQuintic(float t)"
    "{"
    "   return 30.0*t*t*(t*(t-2.0)+1.0);"
    "}"
    ""
    "vec3 PerlinNoise(vec2 pos)"
    "{"
    "   pos = pos * vec2("<<lastGridSize.x<<","<<lastGridSize.y<<");"
    "   vec3 r = vec3(0.0);"
    "   ivec2 ULcorner = ivec2(floor(pos));"
    "   pos = pos - vec2(ULcorner);"
    "   vec2 hermitePos = vec2(Quintic(pos.x),Quintic(pos.y));"
    ""
    "   vec2 rg00 = (vec3(random_gradients[GridPos(ULcorner.x,ULcorner.y)],0.0)).xy;"
    "   vec2 rg10 = (vec3(random_gradients[GridPos(ULcorner.x+1,ULcorner.y)],0.0)).xy;"
    "   vec2 rg11 = (vec3(random_gradients[GridPos(ULcorner.x+1,ULcorner.y+1)],0.0)).xy;"
    "   vec2 rg01 = (vec3(random_gradients[GridPos(ULcorner.x,ULcorner.y+1)],0.0)).xy;"
    ""
    "   float dot00 = dot(rg00,pos);"
    "   float dot10 = dot(rg10,pos-(constantList.xy));"
    "   float dot11 = dot(rg11,pos-(constantList.xx));"
    "   float dot01 = dot(rg01,pos-(constantList.yx));"
    ""
    "   float m0010 = mix(dot00,dot10,hermitePos.x);"
    "   float m0111 = mix(dot01,dot11,hermitePos.x);"
    "   r.z = mix(m0010, m0111,hermitePos.y);"
    ""
    "   r.xy = (1.0-hermitePos.y)*(hermitePos.x*(rg10-rg00)+rg00 + vec2(DQuintic(pos.x)*(dot10-dot00), 0.0))"
    "           + hermitePos.y * (hermitePos.x*(rg11-rg01)+rg01 + vec2(DQuintic(pos.x)*(dot11-dot01), 0.0))"
    "            +(m0111 - m0010)*vec2(0.0, DQuintic(pos.y));"
    ""
    "   return r;"
    "}"
    ""
    "vec3 FractalNoise(vec2 pos)"
    "{"
    "   vec3 r = vec3(0.0);"
    "   float frequency = p_frequency;"
    "   float amplitude = p_amplitude;"
    "   for(int i = 0 ; i < p_fractal ; ++i)"
    "   {"
    "       if(p_useAbs == false)"
    "           r += PerlinNoise(pos*frequency) * amplitude;"
    "       else"
    "           r += abs(PerlinNoise(pos*frequency) * amplitude);"
    "       amplitude *= p_amplitudeMultiplier;"
    "       frequency *= p_frequencyMultiplier;"
    "   }"
    "   return r;"
    "}"
    ""
    "void main()" \
    "{"
    "   vec3 noise = FractalNoise(Coord0);"
    "   gl_FragColor = vec4(noise,1.0);"
    "}";

    m_perlinNoiseShader.loadFromMemory(vertexShader.str(), fragShader.str());
}

}

