#include "ALAGE/gfx/Sprite3DEntity.h"

#include <SFML/Opengl.hpp>
#include "ALAGE/gfx/SceneNode.h"

namespace alag
{

const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 pixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   gl_FragDepth = 1.0 - texture2D(depthMap, gl_TexCoord[0].xy).a*(texture2D(depthMap, gl_TexCoord[0].xy).r*height + zPos);" \
    "   gl_FragColor = gl_Color * pixel; " \
    "}";


Sprite3DEntity::Sprite3DEntity() : Sprite3DEntity(sf::IntRect (0,0,0,0))
{
    //ctor
}

Sprite3DEntity::Sprite3DEntity(const sf::Vector2i &v) : Sprite3DEntity(sf::IntRect (0,0,v.x, v.y))
{
    //ctor
}


Sprite3DEntity::Sprite3DEntity(const sf::IntRect &r) : SpriteEntity(r)
{
}


Sprite3DEntity::~Sprite3DEntity()
{
    //dtor
}

void Sprite3DEntity::Render(sf::RenderTarget *w)
{
    Render(w,sf::Transform::Identity);
}

void Sprite3DEntity::Render(sf::RenderTarget *w, const sf::Transform &t)
{
    if(m_texture != nullptr && m_texture->IsLoaded())
    {
        w->pushGLStates();
       // w->resetGLStates();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        sf::Shader depth;
        depth.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
        depth.setUniform("colorMap",*m_texture->GetColorMap());
        depth.setUniform("depthMap",*m_texture->GetDepthMap());
        depth.setUniform("height",m_texture->GetHeight()*getScale().y*0.001f);

        sf::Vector3f globalPos(0,0,0);

        sf::Transform totalTransform;
        totalTransform = sf::Transform::Identity;
        if(GetParentNode() != nullptr)
        {
            globalPos = GetParentNode()->GetGlobalPosition();
            totalTransform.translate(t.transformPoint(globalPos.x, globalPos.y));
        }

        depth.setUniform("zPos",globalPos.z*0.001f);

        sf::RenderStates state;
        state.transform = totalTransform;
        state.shader = &depth;
        w->draw((*this), state);
        w->popGLStates();
    }
}

void Sprite3DEntity::SetTexture(Texture3DAsset *texture)
{
    m_texture = texture;
    if(texture != nullptr)
        sf::Sprite::setTexture(*(texture->GetColorMap(this)));
}

void Sprite3DEntity::NotifyLoadedAsset(Asset *asset)
{
    if(asset == m_texture)
        //sf::Sprite::setTexture(*(m_texture->GetColorMap()));
        SetTexture(m_texture);
}

}

