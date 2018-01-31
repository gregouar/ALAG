#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/iso/IsometricScene.h"
#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

IsoSpriteEntity::IsoSpriteEntity() : IsoSpriteEntity(sf::IntRect (0,0,0,0))
{
}

IsoSpriteEntity::IsoSpriteEntity(const sf::Vector2i &v) : IsoSpriteEntity(sf::IntRect (0,0,v.x, v.y))
{
}

IsoSpriteEntity::IsoSpriteEntity(const sf::IntRect &r) : SpriteEntity(r)
{
    m_scene = nullptr;
}


IsoSpriteEntity::~IsoSpriteEntity()
{
    //dtor
}


void IsoSpriteEntity::PrepareShader(sf::Shader* shader)
{
    SpriteEntity::PrepareShader(shader);

    if(!Is3D())
    if(m_scene != nullptr)
    {
        float isoToCartZFactor = -m_scene->GetIsoToCartMat().values[5];
        shader->setUniform("useNormalMap", false);
        shader->setUniform("depthMap",*AssetHandler<TextureAsset>::Instance()->LoadAssetFromFile("../data/heightmap.png")->GetTexture());
        shader->setUniform("useDepthMap",true);
        shader->setUniform("height",(-(float)sf::Sprite::getTextureRect().height*(float)getScale().y)/isoToCartZFactor);
    }

    shader->setUniform("normalProjMatInv",sf::Glsl::Mat3(IdMat3X3));
}

void IsoSpriteEntity::RenderShadow(sf::RenderTarget *w/*, const sf::RenderStates &state*/, Light* light)
{
    //w->draw((*this), state);
    SceneNode* node = GetParentNode();

    if(node != nullptr)
    if(m_scene != nullptr)
    if(Is3D())
    //if(m_texture != nullptr && m_texture->IsLoaded())
    {
        Texture3DAsset *myTexture3D = (Texture3DAsset*) m_texture;
        sf::Vector3f globalPos = node->GetGlobalPosition();

        sf::Shader* depthShader = m_scene->GetDepthShader();
        depthShader->setUniform("colorMap",m_shadowMap[light]);
        depthShader->setUniform("depthMap",m_shadowMap[light]);
        depthShader->setUniform("useDepthMap", true);
        depthShader->setUniform("height",myTexture3D->GetHeight()*sf::Sprite::getScale().y);
        depthShader->setUniform("zPos",globalPos.z);


        sf::RenderStates state;
        sf::Vector3f t = m_scene->GetIsoToCartMat()*globalPos;
        state.transform.translate(t.x, t.y);
        state.shader = depthShader;
        w->draw(m_shadowSprite[light], state);
    }
}


void BlurImage()
{

}

void IsoSpriteEntity::ComputeShadow(Light* light)
{
    if(m_scene != nullptr)
    if(m_texture != nullptr
    && m_texture->IsLoaded())
    if(light != nullptr && light->GetParentNode() != nullptr
     &&GetParentNode() != nullptr)
    {
        if(light->GetType() == DirectionnalLight
            && (GetShadowCastingType() == DirectionnalShadow || GetShadowCastingType() == AllShadows))
        if(Is3D())
        if(light->GetDirection().z < 0)
        {
            Texture3DAsset* myTexture3D = (Texture3DAsset*)m_texture;
            float height = myTexture3D->GetHeight()*getScale().y;
            sf::Vector3f lightDirection = Normalize(light->GetDirection());

            Mat3x3 isoToCart = m_scene->GetIsoToCartMat();
            Mat3x3 cartToIso = m_scene->GetCartToIsoMat();

            sf::Vector2f iso_decal(-lightDirection.x*height/lightDirection.z,-lightDirection.y*height/lightDirection.z);
            sf::Vector2f max_decal = isoToCart*iso_decal;

            max_decal.x = (int)max_decal.x +((max_decal.x > 0) ? 1 : -1);
            max_decal.y = (int)max_decal.y +((max_decal.y > 0) ? 1 : -1);

            sf::IntRect shadow_bounds(0,0,getGlobalBounds().width+abs(max_decal.x),
                                            getGlobalBounds().height+abs(max_decal.y));

            if(max_decal.x < 0)
                shadow_bounds.left = max_decal.x;
            if(max_decal.y < 0)
                shadow_bounds.top = max_decal.y;

            sf::Uint8 *shadow_map_array = new sf::Uint8[shadow_bounds.width*shadow_bounds.height*4];

            sf::Texture* depth_texture = myTexture3D->GetDepthMap();
            sf::Image depth_img = depth_texture->copyToImage();
            int depth_texture_width = depth_img.getSize().x;
            const sf::Uint8* depth_array = depth_img.getPixelsPtr();

            for(size_t t = 0 ; t < shadow_bounds.width*shadow_bounds.height ; ++t)
                shadow_map_array[t*4+3] = 0;


            float height_pixel = 0;
            sf::Vector2f proj_pos(0,0);
            for(size_t x = 0 ; x < depth_texture_width ; ++x)
            for(size_t y = 0 ; y < depth_img.getSize().y ; ++y)
            {
                sf::Color color_pixel(depth_array[(x + y*depth_texture_width)*4],
                                      depth_array[(x + y*depth_texture_width)*4+1],
                                      depth_array[(x + y*depth_texture_width)*4+2],
                                      depth_array[(x + y*depth_texture_width)*4+3]);
                height_pixel  = color_pixel.a*0.00392156862*(color_pixel.r + color_pixel.g + color_pixel.b);
                height_pixel *= height*0.00130718954;

                sf::Vector2f pos(x,y);
                pos.y -= height_pixel*isoToCart.values[5];
                pos -= isoToCart*(sf::Vector2f(lightDirection.x/lightDirection.z,
                                               lightDirection.y/lightDirection.z)*height_pixel);

                pos.x = (int)(pos.x+0.5) - shadow_bounds.left;
                pos.y = (int)(pos.y+0.5) - shadow_bounds.top;

                int array_pos = (pos.x + pos.y*shadow_bounds.width)*4;

                if(array_pos < shadow_bounds.width*shadow_bounds.height*4)
                if(color_pixel.a*(color_pixel.r + color_pixel.g + color_pixel.b) >
                   shadow_map_array[array_pos+3]*(shadow_map_array[array_pos]
                                                  +shadow_map_array[array_pos+1]
                                                  +shadow_map_array[array_pos+2]))
                {
                    shadow_map_array[array_pos] = color_pixel.r;
                    shadow_map_array[array_pos+1] = color_pixel.g;
                    shadow_map_array[array_pos+2] = color_pixel.b;
                    shadow_map_array[array_pos+3] = color_pixel.a;
                }
            }

           // BlurImage(shadow_map_array,shadow_bounds.width,shadow_bounds.height,sf::Vector2f(1,0));

            //sf::Image shadow_img;
            //shadow_img.create(shadow_bounds.width,shadow_bounds.height,shadow_map_array);

            /** ADD BLUR **/

            sf::Texture* shadowTexture = &m_shadowMap[light];
            shadowTexture->create(shadow_bounds.width,shadow_bounds.height);
            shadowTexture->update(shadow_map_array,shadow_bounds.width,shadow_bounds.height,0,0);
            //shadowTexture->loadFromImage(shadow_img);
            m_shadowSprite[light].setTexture(*shadowTexture);
            m_shadowSprite[light].setOrigin(sf::Sprite::getOrigin()
                                            -sf::Vector2f(shadow_bounds.left, shadow_bounds.top));


            delete[] shadow_map_array;
        }

        AddToLightList(light);
    }
}

void IsoSpriteEntity::SetIsoScene(IsometricScene *scene)
{
    m_scene = scene;
}


}
