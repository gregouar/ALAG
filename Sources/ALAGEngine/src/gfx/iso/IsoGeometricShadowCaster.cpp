#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/gfx/iso/PBRIsoScene.h"
#include "ALAGE/gfx/iso/IsoGeometricShadowCaster.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

IsoGeometricShadowCaster::IsoGeometricShadowCaster()
{
}

IsoGeometricShadowCaster::~IsoGeometricShadowCaster()
{
    //dtor
}


void IsoGeometricShadowCaster::RenderShadow(sf::RenderTarget *w/*, const sf::RenderStates &state*/, Light* light)
{
    std::map<Light*, sf::Drawable*>::iterator shadowIt;
    shadowIt = m_shadowDrawable.find(light);
    if(shadowIt != m_shadowDrawable.end())
    {
        SceneNode* node = GetParentNode();

        if(node != nullptr && m_scene != nullptr)
        {
            sf::Vector3f globalPos = node->GetGlobalPosition();

            sf::Shader* depthShader = m_scene->GetDepthShader();
            //depthShader->setUniform("map_color",m_shadowMap[light]);
            depthShader->setUniform("enable_albedoMap", false);
          //  depthShader->setUniform("map_depth",m_shadowMap[light]);
            depthShader->setUniform("enable_depthMap", false);
            depthShader->setUniform("enable_depthColor", true);
            depthShader->setUniform("p_height",m_height*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
            depthShader->setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);

            /*sf::Vector3f light_direction = Normalize(light->GetDirection());
            globalPos -= sf::Vector3f(globalPos.z*light_direction.x/light_direction.z,
                                      globalPos.z*light_direction.y/light_direction.z, 0);
            globalPos.z = 0;*/

            sf::RenderStates state;


           /** sf::Vector3f t = m_scene->GetIsoToCartMat()*globalPos;
            state.transform.translate(t.x, t.y);**/

            sf::Vector3f v = m_scene->GetIsoToCartMat()*globalPos;

            state.transform = sf::Transform::Identity;
            state.transform.translate(v.x, v.y);
            state.transform *= m_scene->GetIsoToCartTransform();

            state.shader = depthShader;
            w->draw(*shadowIt->second, state);
        }
    }
}

void IsoGeometricShadowCaster::SetIsoScene(PBRIsoScene *scene)
{
    m_scene = scene;
}


}
