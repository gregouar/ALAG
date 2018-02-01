#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/iso/IsometricScene.h"
#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/utils/TextureModifier.h"

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
        shader->setUniform("enable_normalMap", false);
        shader->setUniform("map_depth",*AssetHandler<TextureAsset>::Instance()->LoadAssetFromFile("../data/heightmap.png")->GetTexture());
        shader->setUniform("enable_depthMap",true);
        shader->setUniform("p_height",(-(float)sf::Sprite::getTextureRect().height*(float)getScale().y)/isoToCartZFactor);
    }

    shader->setUniform("p_normalProjMatInv",sf::Glsl::Mat3(IdMat3X3));
}

void IsoSpriteEntity::RenderShadow(sf::RenderTarget *w/*, const sf::RenderStates &state*/, Light* light)
{
    SceneNode* node = GetParentNode();

    if(Is3D())
    if(node != nullptr && m_scene != nullptr)
    if(m_texture != nullptr && m_texture->IsLoaded())
    {
        Texture3DAsset *myTexture3D = (Texture3DAsset*) m_texture;
        sf::Vector3f globalPos = node->GetGlobalPosition();

        sf::Shader* depthShader = m_scene->GetDepthShader();
        depthShader->setUniform("map_color",m_shadowMap[light]);
        depthShader->setUniform("map_depth",m_shadowMap[light]);
        depthShader->setUniform("enable_depthMap", true);
        depthShader->setUniform("p_height",myTexture3D->GetHeight()*sf::Sprite::getScale().y);
        depthShader->setUniform("p_zPos",globalPos.z);

        sf::Vector3f light_direction = Normalize(light->GetDirection());
        globalPos -= sf::Vector3f(globalPos.z*light_direction.x/light_direction.z,
                                  globalPos.z*light_direction.y/light_direction.z, 0);
        globalPos.z = 0;

        sf::RenderStates state;
        sf::Vector3f t = m_scene->GetIsoToCartMat()*globalPos;
        state.transform.translate(t.x, t.y);
        state.shader = depthShader;
        w->draw(m_shadowSprite[light], state);
    }
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
            //Mat3x3 cartToIso = m_scene->GetCartToIsoMat();

            sf::Vector2f iso_shift(-lightDirection.x*height/lightDirection.z,
                                   -lightDirection.y*height/lightDirection.z);

            sf::Vector2f max_shift = isoToCart*iso_shift;
            max_shift.y -= height*isoToCart.values[5];

            max_shift.x = (int)max_shift.x +((max_shift.x > 0) ? 1 : -1);
            max_shift.y = (int)max_shift.y +((max_shift.y > 0) ? 1 : -1);

            sf::IntRect shadow_bounds(0,0,getGlobalBounds().width+abs(max_shift.x),
                                          getGlobalBounds().height+abs(max_shift.y));

            if(max_shift.x < 0)
                shadow_bounds.left = max_shift.x;
            if(max_shift.y < 0)
                shadow_bounds.top = max_shift.y;

            m_shadowMaxShift[light] = sf::IntRect(shadow_bounds.left,shadow_bounds.top,
                                                  abs(max_shift.x), abs(max_shift.y));

            sf::Uint8 *shadow_map_array = new sf::Uint8[shadow_bounds.width*shadow_bounds.height*4];

            sf::Texture* depth_texture = myTexture3D->GetDepthMap();
            sf::Image depth_img = depth_texture->copyToImage();
            size_t depth_texture_width = depth_img.getSize().x;
            const sf::Uint8* depth_array = depth_img.getPixelsPtr();

            for(size_t t = 0 ; t < (size_t)(shadow_bounds.width*shadow_bounds.height) ; ++t)
            {
                shadow_map_array[t*4] = 0;
                shadow_map_array[t*4+1] = 0;
                shadow_map_array[t*4+2] = 0;
                shadow_map_array[t*4+3] = 0;
            }


            float height_pixel = 0;
            sf::Vector2f proj_pos(0,0);
            for(size_t x = 0 ; x < depth_texture_width ; ++x)
            for(size_t y = 0 ; y < depth_img.getSize().y ; ++y)
            if(depth_array[(x + y*depth_texture_width)*4+3] == 255)
            {
                sf::Color color_pixel(depth_array[(x + y*depth_texture_width)*4],
                                      depth_array[(x + y*depth_texture_width)*4+1],
                                      depth_array[(x + y*depth_texture_width)*4+2],
                                      depth_array[(x + y*depth_texture_width)*4+3]);
                height_pixel  =  /*color_pixel.a*0.00392156862* */(color_pixel.r + color_pixel.g + color_pixel.b);
                height_pixel *= height*0.00130718954;

                sf::Vector2f pos(x,y);
                pos.y -= height_pixel*isoToCart.values[5];
                pos -= isoToCart*(sf::Vector2f(lightDirection.x/lightDirection.z,
                                               lightDirection.y/lightDirection.z)*height_pixel);

                pos.x = (int)(pos.x+0.5) - shadow_bounds.left;
                pos.y = (int)(pos.y+0.5) - shadow_bounds.top;

                for(int dx = -1 ; dx <= 1 ; ++dx)
                for(int dy = -1 ; dy <= 1 ; ++dy)
                {
                    int array_pos = ((pos.x+dx) + (pos.y+dy)*shadow_bounds.width)*4;

                    if(array_pos >= 0 && array_pos < shadow_bounds.width*shadow_bounds.height*4)
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
            }

            sf::Texture* shadowTexture = &m_shadowMap[light];
            shadowTexture->create(shadow_bounds.width,shadow_bounds.height);
            shadowTexture->update(shadow_map_array,shadow_bounds.width,shadow_bounds.height,0,0);
            TextureModifier::BlurTexture(shadowTexture, 5);
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
