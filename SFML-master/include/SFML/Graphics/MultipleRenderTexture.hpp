#ifndef MULTIPLERENDERTEXTURE_H
#define MULTIPLERENDERTEXTURE_H

#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Context.hpp>

namespace sf
{

class SFML_GRAPHICS_API MultipleRenderTexture : public RenderTarget, GlResource
{
public:

    MultipleRenderTexture();
    virtual ~MultipleRenderTexture();

    bool create(unsigned int width, unsigned int height, bool depthBuffer = false);
    bool addRenderTarget(unsigned int renderingLocation);
    bool removeRenderTarget(unsigned int renderingLocation);
    //bool create(unsigned int width, unsigned int height, bool depthBuffer = false);

    /*void setSmooth(bool smooth);
    bool isSmooth() const;
    void setRepeated(bool repeated);
    bool isRepeated() const;
    bool generateMipmap();*/

    bool setActive(bool active);
    void display();

    virtual sf::Vector2u getSize() const;
    const Texture& getTexture(unsigned int renderingLocation);


protected:
    std::vector<unsigned int>::iterator findRenderTarget(unsigned int renderingLocation);
    bool createFBO(unsigned int width, unsigned int height, bool depthBuffer = false);
    bool addTargetToFBO(unsigned int renderingLocation, unsigned int);
    //void UpdateFBO();

private:
    //priv::MultipleRenderTextureImpl* m_impl;
    Texture *m_textures;
    std::vector<unsigned int> m_activeTextures;
    //unsigned int *m_attachments;
    Vector2u m_size;

    Context*     m_context;
    unsigned int m_frameBuffer;
    unsigned int m_depthBuffer;
};

}


#endif // MULTIPLERENDERTEXTURE_H
