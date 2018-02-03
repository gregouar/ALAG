

#include <SFML/Graphics/MultipleRenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/System/Err.hpp>



namespace sf
{

unsigned int MultipleRenderTexture::getMaxColorAttachments()
{
    GLint maxAttach = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach);
    return maxAttach;
}

MultipleRenderTexture::MultipleRenderTexture() :
    m_context    (NULL),
    m_frameBuffer(0),
    m_depthBuffer(0)
{
    m_size = Vector2u(0,0);
    m_textures = NULL;
}

MultipleRenderTexture::~MultipleRenderTexture()
{
    if(m_context != NULL)
    {
        m_context->setActive(true);

        if (m_depthBuffer)
        {
            GLuint depthBuffer = static_cast<GLuint>(m_depthBuffer);
            glCheck(GLEXT_glDeleteFramebuffers(1, &depthBuffer));
        }

        if (m_frameBuffer)
        {
            GLuint frameBuffer = static_cast<GLuint>(m_frameBuffer);
            glCheck(GLEXT_glDeleteFramebuffers(1, &frameBuffer));
        }

        delete m_context;
    }

    if(m_textures != NULL)
        delete m_textures;
}


bool MultipleRenderTexture::create(unsigned int width, unsigned int height, bool depthBuffer)
{
    m_size = Vector2u(width, height);

    if(m_textures != NULL)
        delete m_textures;

    m_textures = new Texture[getMaxColorAttachments()];

    if(m_context != NULL)
        delete m_context;

    m_context = new Context();

    if(!createFBO(width,height,depthBuffer))
        return false;

    if(!addRenderTarget(0))
        return false;

    return true;
}

bool MultipleRenderTexture::addRenderTarget(unsigned int renderingLocation)
{
    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
    {
        err()<< "Impossible to add render target (location already taken)" <<std::endl;
        return false;
    }

    if(renderingLocation >= getMaxColorAttachments())
    {
        err()<< "Impossible to add render target (location not available)" <<std::endl;
        return false;
    }

    Texture* curTexture = &m_textures[renderingLocation];

    if (!curTexture->create(m_size.x, m_size.y))
    {
        err() << "Impossible to add render texture (failed to create the target texture)" << std::endl;
        return false;
    }

    curTexture->setSmooth(false);

    /*// Create the implementation
    delete m_impl;
    if (priv::RenderTextureImplFBO::isAvailable())
    {
        // Use frame-buffer object (FBO)
        m_impl = new priv::RenderTextureImplFBO;

        // Mark the texture as being a framebuffer object attachment
        m_texture.m_fboAttachment = true;
    }
    else
    {
        // Use default implementation
        m_impl = new priv::RenderTextureImplDefault;
    }*/

    curTexture->m_fboAttachment = true;

    if (!addTargetToFBO(renderingLocation,curTexture->m_texture))
        return false;

    m_activeTextures.push_back(renderingLocation+GL_COLOR_ATTACHMENT0_EXT);

    return true;
}



bool MultipleRenderTexture::removeRenderTarget(unsigned int renderingLocation)
{
    if(m_activeTextures.size() <= 1)
    {
        err() << "Impossible to remove render texture (cannot remove all render textures)" << std::endl;
        return false;
    }

    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);
    if(renderIt == m_activeTextures.end())
    {
        err() << "Impossible to remove render texture (cannot find render texture)" << std::endl;
        return false;
    }

    m_activeTextures.erase(renderIt);
    return true;
}

void MultipleRenderTexture::setSmooth(unsigned int renderingLocation, bool smooth)
{
    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
        m_textures[*renderIt-GL_COLOR_ATTACHMENT0_EXT].setSmooth(smooth);
}

bool MultipleRenderTexture::isSmooth(unsigned int renderingLocation)
{
    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
        return m_textures[*renderIt-GL_COLOR_ATTACHMENT0_EXT].isSmooth();
    return false;
}

void MultipleRenderTexture::setRepeated(unsigned int renderingLocation, bool repeated)
{

    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
        m_textures[*renderIt-GL_COLOR_ATTACHMENT0_EXT].setRepeated(repeated);
}

bool MultipleRenderTexture::isRepeated(unsigned int renderingLocation)
{

    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
        return m_textures[*renderIt-GL_COLOR_ATTACHMENT0_EXT].isRepeated();
    return false;
}

bool MultipleRenderTexture::generateMipmap(unsigned int renderingLocation)
{

    std::vector<unsigned int>::iterator renderIt;
    renderIt = findRenderTarget(renderingLocation);

    if(renderIt != m_activeTextures.end())
        m_textures[*renderIt-GL_COLOR_ATTACHMENT0_EXT].generateMipmap();
}

bool MultipleRenderTexture::setActive(bool active)
{
    if(m_context == NULL)
        return false;

    if(!m_context->setActive(active))
        return false;

    if(active)
        GLEXT_glDrawBuffers(m_activeTextures.size(),m_activeTextures.data());

    return true;
}

void MultipleRenderTexture::display()
{
    if (setActive(true))
    {
        glCheck(glFlush());

        std::vector<unsigned int>::iterator it;
        for(it = m_activeTextures.begin() ; it != m_activeTextures.end() ; ++it)
        {
            m_textures[*it-GL_COLOR_ATTACHMENT0_EXT].m_pixelsFlipped = true;
            m_textures[*it-GL_COLOR_ATTACHMENT0_EXT].invalidateMipmap();
        }

    }
}


Vector2u MultipleRenderTexture::getSize() const
{
    return m_size;
}

Texture* MultipleRenderTexture::getTexture(unsigned int renderingLocation)
{
    if(renderingLocation >= getMaxColorAttachments())
    {
        err()<< "Impossible to get texture from multiple render texture (location not available)" <<std::endl;
        return NULL;
    }

    return &m_textures[renderingLocation];
}


std::vector<unsigned int>::iterator MultipleRenderTexture::findRenderTarget(unsigned int renderingLocation)
{
    std::vector<unsigned int>::iterator it;
    it = m_activeTextures.begin();

    while(it != m_activeTextures.end() && *it != renderingLocation + GL_COLOR_ATTACHMENT0_EXT)
        ++it;

    return it;
}

bool MultipleRenderTexture::createFBO(unsigned int width, unsigned int height, bool depthBuffer)
{
    GLuint frameBuffer = 0;
    glCheck(GLEXT_glGenFramebuffers(1, &frameBuffer));
    m_frameBuffer = static_cast<unsigned int>(frameBuffer);
    if (!m_frameBuffer)
    {
        err() << "Impossible to create mutliple render texture (failed to create the frame buffer object)" << std::endl;
        return false;
    }
    glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, m_frameBuffer));

    if (depthBuffer)
    {
        GLuint depth = 0;
        glCheck(GLEXT_glGenRenderbuffers(1, &depth));
        m_depthBuffer = static_cast<unsigned int>(depth);
        if (!m_depthBuffer)
        {
            err() << "Impossible to create render texture (failed to create the attached depth buffer)" << std::endl;
            return false;
        }
        glCheck(GLEXT_glBindRenderbuffer(GLEXT_GL_RENDERBUFFER, m_depthBuffer));
        glCheck(GLEXT_glRenderbufferStorage(GLEXT_GL_RENDERBUFFER, GLEXT_GL_DEPTH_COMPONENT, width, height));
        glCheck(GLEXT_glFramebufferRenderbuffer(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_DEPTH_ATTACHMENT, GLEXT_GL_RENDERBUFFER, m_depthBuffer));
    }

    RenderTarget::initialize();
}


bool MultipleRenderTexture::addTargetToFBO(unsigned int renderingLocation, unsigned int textureId)
{
    if(renderingLocation >= getMaxColorAttachments())
    {
        err()<< "Impossible to add render target (location not available)" <<std::endl;
        return false;
    }

    glCheck(GLEXT_glFramebufferTexture2D(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_COLOR_ATTACHMENT0+renderingLocation
                                         , GL_TEXTURE_2D, textureId, 0));

    // A final check, just to be sure...
    GLenum status;
    glCheck(status = GLEXT_glCheckFramebufferStatus(GLEXT_GL_FRAMEBUFFER));
    if (status != GLEXT_GL_FRAMEBUFFER_COMPLETE)
    {
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, 0));
        err() << "Impossible to create render texture (failed to link the target texture to the frame buffer)" << std::endl;
        return false;
    }

    return true;
}

/*void MultipleRenderTexture::UpdateFBO()
{
        size_t nbrTextures = m_activeTextures.size();
        unsigned int *attachments = new unsigned int[nbrTextures];

        for(size_t i = 0 ; i < nbrTextures ; ++i)
            attachments[i] = m_activeTextures[i];

        attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
}*/


/*
////////////////////////////////////////////////////////////
bool RenderTexture::create(unsigned int width, unsigned int height, bool depthBuffer)
{
    // Create the texture
    if (!m_texture.create(width, height))
    {
        err() << "Impossible to create render texture (failed to create the target texture)" << std::endl;
        return false;
    }

    // We disable smoothing by default for render textures
    setSmooth(false);

    // Create the implementation
    delete m_impl;
    if (priv::RenderTextureImplFBO::isAvailable())
    {
        // Use frame-buffer object (FBO)
        m_impl = new priv::RenderTextureImplFBO;

        // Mark the texture as being a framebuffer object attachment
        m_texture.m_fboAttachment = true;
    }
    else
    {
        // Use default implementation
        m_impl = new priv::RenderTextureImplDefault;
    }

    // Initialize the render texture
    if (!m_impl->create(width, height, m_texture.m_texture, depthBuffer))
        return false;

    // We can now initialize the render target part
    RenderTarget::initialize();

    return true;
}


////////////////////////////////////////////////////////////
void RenderTexture::setSmooth(bool smooth)
{
    m_texture.setSmooth(smooth);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isSmooth() const
{
    return m_texture.isSmooth();
}


////////////////////////////////////////////////////////////
void RenderTexture::setRepeated(bool repeated)
{
    m_texture.setRepeated(repeated);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isRepeated() const
{
    return m_texture.isRepeated();
}


////////////////////////////////////////////////////////////
bool RenderTexture::generateMipmap()
{
    return m_texture.generateMipmap();
}


////////////////////////////////////////////////////////////
bool RenderTexture::setActive(bool active)
{
    return m_impl && m_impl->activate(active);
}


////////////////////////////////////////////////////////////
void RenderTexture::display()
{
    // Update the target texture
    if (setActive(true))
    {
        m_impl->updateTexture(m_texture.m_texture);
        m_texture.m_pixelsFlipped = true;
        m_texture.invalidateMipmap();
    }
}


////////////////////////////////////////////////////////////
Vector2u RenderTexture::getSize() const
{
    return m_texture.getSize();
}


////////////////////////////////////////////////////////////
const Texture& RenderTexture::getTexture() const
{
    return m_texture;
}*/

}
