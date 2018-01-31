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

void IsoSpriteEntity::ComputeShadow(Light* light)
{
            std::cout<<"hoho"<<std::endl;
    if(m_scene != nullptr)
    if(m_texture != nullptr
    && m_texture->IsLoaded())
    if(light != nullptr && light->GetParentNode() != nullptr
     &&GetParentNode() != nullptr)
    {
        if(light->GetType() == DirectionnalLight
            && (GetShadowCastingType() == DirectionnalShadow || GetShadowCastingType() == AllShadows))
        if(Is3D())
        {
            Texture3DAsset* myTexture3D = (Texture3DAsset*)m_texture;
            float height = myTexture3D->GetHeight()*getScale().y;
            sf::Vector3f lightDirection = Normalize(light->GetDirection());
            sf::Vector3f casterPos = GetParentNode()->GetGlobalPosition();

            sf::Vector2f upperLeft_bound(0,0);
            sf::Vector2f upperRight_bound(getGlobalBounds().width,0);
            sf::Vector2f lowerLeft_bound(0,getGlobalBounds().height);
            sf::Vector2f lowerRight_bound(getGlobalBounds().width,
                                          getGlobalBounds().height);

            //Mat3x3 isoToCart = m_scene->GetIsoToCartTransform();
            Mat3x3 cartToIso = m_scene->GetCartToIsoMat();

            sf::Vector2f isoUL_bound = cartToIso*upperLeft_bound;
            sf::Vector2f isoUR_bound = cartToIso*upperRight_bound;
            sf::Vector2f isoLL_bound = cartToIso*lowerLeft_bound;
            sf::Vector2f isoLR_bound = cartToIso*lowerRight_bound;

            if(lightDirection.x > 0)
            {
                isoUR_bound.x += height*lightDirection.x;
                isoLR_bound.x += height*lightDirection.x;
            } else {
                isoUL_bound.x += height*lightDirection.x;
                isoLL_bound.x += height*lightDirection.x;
            }

            if(lightDirection.y > 0)
            {
                isoLL_bound.y += height*lightDirection.y;
                isoLR_bound.y += height*lightDirection.y;
            } else {
                isoUL_bound.y += height*lightDirection.y;
                isoUR_bound.y += height*lightDirection.y;
            }

            int shadow_width = isoLR_bound.x - isoUL_bound.x;
            int shadow_height = isoLR_bound.y - isoUL_bound.y;

            sf::Uint8 *shadow_map_pix = new sf::Uint8[shadow_width*shadow_height*4];

            const sf::Uint8* depthmap = myTexture3D->GetDepthMap()->copyToImage().getPixelsPtr();

            sf::Image shadow_img;
            shadow_img.create(shadow_width,shadow_height,shadow_map_pix);
            shadow_img.saveToFile("testshadow.png");

            sf::Texture* shadowTexture = &m_shadowMap[light];
            shadowTexture->loadFromImage(shadow_img);
            m_shadowSprite[light].setTexture(*shadowTexture);


            delete[] shadow_map_pix;
        }

        AddToLightList(light);
    }
}

void IsoSpriteEntity::SetIsoScene(IsometricScene *scene)
{
    m_scene = scene;
}


}
