#include "ALAGE/gfx/GeometricShadowCaster.h"

#include "ALAGE/gfx/SceneNode.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

GeometricShadowCaster::GeometricShadowCaster()
{
    m_isAShadowCaster = true;
    ForceShadowCastingType(AllShadows);
}

GeometricShadowCaster::~GeometricShadowCaster()
{
    //dtor
}


/*void GeometricShadowCaster::RenderShadow(sf::RenderTarget *w, Light* light)
{
    SceneNode* node = GetParentNode();

    if(node != nullptr && m_scene != nullptr)
    {
        sf::Vector3f globalPos = node->GetGlobalPosition();

        sf::Shader* depthShader = m_scene->GetDepthShader();
        //depthShader->setUniform("map_color",m_shadowMap[light]);
        depthShader->setUniform("map_depth",m_shadowMap[light]);
        depthShader->setUniform("enable_depthMap", true);
        depthShader->setUniform("p_height",m_height);
        depthShader->setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);

        sf::RenderStates state;
        sf::Vector3f t = m_scene->GetIsoToCartMat()*globalPos;
        state.transform.translate(t.x, t.y);
        state.shader = depthShader;
        w->draw(*m_shadowDrawable[light], state);
    }
}*/

void GeometricShadowCaster::ComputeShadow(Light* light)
{
    if(light != nullptr && light->GetParentNode() != nullptr
     &&GetParentNode() != nullptr)
    {
        if(light->GetType() == OmniLight
            && (GetShadowCastingType() == DynamicShadow || GetShadowCastingType() == AllShadows))
        {
            sf::Vector3f light_pos = light->GetParentNode()->GetGlobalPosition();
            sf::Vector3f global_pos = GetParentNode()->GetGlobalPosition();

            std::map<Light*, sf::Drawable*>::iterator shadowIt;
            shadowIt = m_shadowDrawable.find(light);
            if(shadowIt != m_shadowDrawable.end())
                delete m_shadowDrawable[light];
            sf::VertexArray *vArray = new sf::VertexArray(sf::TriangleStrip,
                                                          m_geometry.getVertexCount()*2);

            for(size_t i = 0 ; i < m_geometry.getVertexCount() ; ++i)
            {
                sf::Vector3f cur_pos = global_pos;
                cur_pos.x += m_geometry[i].position.x;
                cur_pos.y += m_geometry[i].position.y;
                cur_pos.z += (m_geometry[i].color.r + m_geometry[i].color.g + m_geometry[i].color.b + m_geometry[i].color.a)
                                *m_height*0.00098039215;//0.00130718954;

                sf::Vector3f difference = cur_pos - light_pos;
                sf::Vector3f direction = Normalize(difference);
                float radius = light->GetRadius();
                sf::Vector3f xyDifference = difference;
                xyDifference.z = 0;
                float xyDistance = sqrt(ComputeDotProduct(xyDifference, xyDifference));


                (*vArray)[2*i].position = m_geometry[i].position;
                (*vArray)[2*i].color = m_geometry[i].color;

                int r=0,g=0,b=0,a=0;

                if(direction.z >= 0)
                {
                    sf::Vector3f xyDirection = Normalize(sf::Vector3f(direction.x, direction.y,0));


                    sf::Vector3f global_projection = light_pos + xyDirection*radius;

                    (*vArray)[2*i+1].position.x = global_projection.x - global_pos.x;
                    (*vArray)[2*i+1].position.y = global_projection.y - global_pos.y;

                    r = (int)((float)m_geometry[i].color.r*radius/xyDistance);
                } else if (direction.z < 0) {

                    float d = -light_pos.z/difference.z;

                    sf::Vector2f ground_intersection;

                    ground_intersection.x =  light_pos.x + difference.x * d;
                    ground_intersection.y =  light_pos.y + difference.y * d;

                    (*vArray)[2*i+1].position.x = ground_intersection.x - global_pos.x;
                    (*vArray)[2*i+1].position.y = ground_intersection.y - global_pos.y;

                    r = 0;
                }


                if(r > 255)
                {
                    g = r - 255;
                    if(g > 255)
                    {
                        b = g -255;
                            if( b > 255)
                            {
                                a = b - 255;
                                if(a > 255)
                                    a = 255;
                                b = 255;
                            }
                        g = 255;
                    }
                    r = 255;
                }

                (*vArray)[2*i+1].color.r = r;
                (*vArray)[2*i+1].color.g = g;
                (*vArray)[2*i+1].color.b = b;
                (*vArray)[2*i+1].color.a = a;

            }

            m_shadowDrawable[light] = vArray;
        }

        AddToLightList(light);
    }
}


void GeometricShadowCaster::ConstructCube(float x, float y, float z)
{
    m_geometry = sf::VertexArray(sf::LineStrip,5);

    m_geometry[0].position = sf::Vector2f(0, 0);
    m_geometry[1].position = sf::Vector2f(x, 0);
    m_geometry[2].position = sf::Vector2f(x, y);
    m_geometry[3].position = sf::Vector2f(0, y);
    m_geometry[4].position = sf::Vector2f(0, 0);

    m_geometry[0].color = sf::Color(255,0,0,0);
    m_geometry[1].color = sf::Color(255,0,0,0);
    m_geometry[2].color = sf::Color(255,0,0,0);
    m_geometry[3].color = sf::Color(255,0,0,0);
    m_geometry[4].color = sf::Color(255,0,0,0);

    m_height = z*4;
}

void GeometricShadowCaster::ConstructCylinder(float r, float z, unsigned int q)
{
    m_geometry = sf::VertexArray(sf::LineStrip,q+1);

    for(size_t i = 0 ; i < q ; ++i)
    {
        m_geometry[i].position = sf::Vector2f(r*cos(2*PI*(float)i/(float)q),
                                              r*sin(2*PI*(float)i/(float)q));
        m_geometry[i].color = sf::Color(255,0,0,255);
    }
    m_geometry[q].position = sf::Vector2f(r*cos(0),r*sin(0));
    m_geometry[q].color = sf::Color(255,0,0,255);

    m_height = z*4;
}

void GeometricShadowCaster::SetGeometry(sf::VertexArray vArray , float height)
{
    m_geometry = vArray;
    m_height = height;
}

}
