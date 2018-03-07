#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/iso/PBRIsoScene.h"
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
    m_shadowVolumeType = OneSidedShadow;
}


IsoSpriteEntity::~IsoSpriteEntity()
{
    //dtor
}


void IsoSpriteEntity::PrepareShader(sf::Shader* shader)
{
    SpriteEntity::PrepareShader(shader);

    if(!UsePBR())
    if(m_scene != nullptr)
    {
        float isoToCartZFactor = -m_scene->GetIsoToCartMat().values[5];
        shader->setUniform("enable_normalMap", false);
        shader->setUniform("map_depth",*AssetHandler<TextureAsset>::Instance()->LoadAssetFromFile("../data/heightmap.png")->GetTexture());
        shader->setUniform("enable_depthMap",true);
        shader->setUniform("p_height",(-(float)sf::Sprite::getTextureRect().height*(float)getScale().y)
                                        /isoToCartZFactor*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
    }

    //shader->setUniform("p_normalProjMatInv",sf::Glsl::Mat3(IdMat3X3));
}

void IsoSpriteEntity::RenderShadow(sf::RenderTarget *w/*, const sf::RenderStates &state*/, Light* light)
{
    SceneNode* node = GetParentNode();

    if(UsePBR())
    if(node != nullptr && m_scene != nullptr)
    if(m_texture != nullptr && m_texture->IsLoaded())
    {
        PBRTextureAsset *myPBRTexture = (PBRTextureAsset*) m_texture;
        sf::Vector3f globalPos = node->GetGlobalPosition();

        sf::Shader* depthShader = m_scene->GetDepthShader();
        //depthShader->setUniform("map_color",m_shadowMap[light]);
        depthShader->setUniform("enable_albedoMap", false);
        depthShader->setUniform("map_depth",m_shadowMap[light]);
        depthShader->setUniform("enable_depthMap", true);
        depthShader->setUniform("p_height",myPBRTexture->GetHeight()*sf::Sprite::getScale().y*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
        depthShader->setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);

        sf::Vector3f light_direction = Normalize(light->GetDirection());
        globalPos -= sf::Vector3f(globalPos.z*light_direction.x/light_direction.z,
                                  globalPos.z*light_direction.y/light_direction.z, 0);
        globalPos.z = 0;

        sf::RenderStates state;
        sf::Vector3f t = m_scene->GetIsoToCartMat()*globalPos;
        state.transform.translate(t.x, t.y);
        state.shader = depthShader;
        w->draw(*m_shadowDrawable[light], state);
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
        if(UsePBR())
        if(light->GetDirection().z < 0)
        {
            PBRTextureAsset* myPBRTexture = (PBRTextureAsset*)m_texture;
            float height = myPBRTexture->GetHeight()*getScale().y;
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

            sf::Texture* depth_texture = myPBRTexture->GetDepthMap();
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



            sf::Vector2i shrinked_shadow_UL(shadow_bounds.width,shadow_bounds.height),
                         shrinked_shadow_LR(0,0);
            int blur_radius = 5;

            float height_pixel = 0;
            sf::Vector2f proj_pos(0,0);
            for(size_t x = 0 ; x < depth_texture_width ; ++x)
            for(size_t y = 0 ; y < depth_img.getSize().y ; ++y)
            if(depth_array[(x + y*depth_texture_width)*4+3] > 64)
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

                        if(pos.x + dx - blur_radius < shrinked_shadow_UL.x)
                            shrinked_shadow_UL.x = pos.x + dx - blur_radius;
                        if(pos.x + dx + blur_radius > shrinked_shadow_LR.x)
                            shrinked_shadow_LR.x = pos.x + dx + blur_radius;
                        if(pos.y + dy - blur_radius < shrinked_shadow_UL.y)
                            shrinked_shadow_UL.y = pos.y + dy - blur_radius;
                        if(pos.y + dy + blur_radius > shrinked_shadow_LR.y)
                            shrinked_shadow_LR.y = pos.y + dy + blur_radius;
                    }
                }

                if(m_shadowVolumeType == TwoSidedShadow ||
                   m_shadowVolumeType == MirroredTwoSidedShadow)
                {
                    pos = sf::Vector2f(x,y);
                    pos.y -= height_pixel*isoToCart.values[5];
                    pos.y = 2*sf::Sprite::getOrigin().y - pos.y;

                    if(m_shadowVolumeType == MirroredTwoSidedShadow)
                        pos.x = 2*sf::Sprite::getOrigin().x - pos.x;

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

                            if(pos.x + dx - blur_radius < shrinked_shadow_UL.x)
                                shrinked_shadow_UL.x = pos.x + dx - blur_radius;
                            if(pos.x + dx + blur_radius > shrinked_shadow_LR.x)
                                shrinked_shadow_LR.x = pos.x + dx + blur_radius;
                            if(pos.y + dy - blur_radius < shrinked_shadow_UL.y)
                                shrinked_shadow_UL.y = pos.y + dy - blur_radius;
                            if(pos.y + dy + blur_radius > shrinked_shadow_LR.y)
                                shrinked_shadow_LR.y = pos.y + dy + blur_radius;
                        }
                    }
                }
            }


            sf::IntRect shrinked_shadow_bounds(shrinked_shadow_UL.x,shrinked_shadow_UL.y,
                                               shrinked_shadow_LR.x - shrinked_shadow_UL.x,
                                               shrinked_shadow_LR.y - shrinked_shadow_UL.y);

            sf::Image shadowImg, shadowImgShrinked;
            shadowImg.create(shadow_bounds.width,shadow_bounds.height,shadow_map_array);
            shadowImgShrinked.create(shrinked_shadow_bounds.width, shrinked_shadow_bounds.height);
            shadowImgShrinked.copy(shadowImg, 0, 0, shrinked_shadow_bounds);

            sf::Texture* shadowTexture = &m_shadowMap[light];
            shadowTexture->loadFromImage(shadowImgShrinked);
           // shadowTexture->create(shadow_bounds.width,shadow_bounds.height);
            //shadowTexture->update(shadow_map_array,shadow_bounds.width,shadow_bounds.height,0,0);
            TextureModifier::BlurTexture(shadowTexture, blur_radius);
            //shadowTexture->setSmooth(true);

            std::map<Light*, sf::Drawable*>::iterator shadowIt;
            shadowIt = m_shadowDrawable.find(light);
            if(shadowIt != m_shadowDrawable.end())
                delete m_shadowDrawable[light];

            sf::Sprite *sprite = new sf::Sprite();

            sprite->setTexture(*shadowTexture);
            sprite->setOrigin(sf::Sprite::getOrigin()
                                            -sf::Vector2f(shadow_bounds.left+shrinked_shadow_bounds.left,
                                                          shadow_bounds.top+shrinked_shadow_bounds.top));

            m_shadowDrawable[light] = sprite;

            delete[] shadow_map_array;
        }

        AddToLightList(light);
    }
}

ShadowVolumeType IsoSpriteEntity::GetShadowVolumeType()
{
    return m_shadowVolumeType;
}

void IsoSpriteEntity::SetShadowVolumeType(ShadowVolumeType type)
{
    m_shadowVolumeType = type;
}

void IsoSpriteEntity::SetIsoScene(PBRIsoScene *scene)
{
    m_scene = scene;
}


}
