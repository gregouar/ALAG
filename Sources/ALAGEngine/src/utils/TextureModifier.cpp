#include "ALAGE/utils/TextureModifier.h"

#include <SFML/Graphics.hpp>

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
    "uniform sampler2D texture;" \
    "uniform vec2 offset;" \
    "void main()" \
    "{" \
    "    gl_FragColor =  gl_Color * "
	"			   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.000003 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.8)	* 0.000229 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.6)	* 0.005977 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.4)	* 0.060598 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.2)	* 0.24173 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 0.382925 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0)	* 0.000003 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.8)	* 0.000229 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.6)	* 0.005977 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.4)	* 0.060598 + "
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 0.24173); "
    "}";


TextureModifier::TextureModifier()
{
    m_blurShader.loadFromMemory(blur_fragShader,sf::Shader::Fragment);
    m_blurShader.setUniform("texture", sf::Shader::CurrentTexture);
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

    blurShader->setUniform("offset",sf::Vector2f(blur/texture->getSize().x,0));
    renderer.draw(rect,blurShader);
    renderer.display();

    rect.setTexture(&renderer.getTexture());
    blurShader->setUniform("offset",sf::Vector2f(0,blur/texture->getSize().y));
    renderer.draw(rect,blurShader);
    renderer.display();

    texture->update(renderer.getTexture().copyToImage());
}



}

