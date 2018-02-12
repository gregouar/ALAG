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


void GeometricShadowCaster::RenderShadow(sf::RenderTarget *w/*, const sf::RenderStates &state*/, Light* light)
{
    /*SceneNode* node = GetParentNode();

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
    }*/
}

void GeometricShadowCaster::ComputeShadow(Light* light)
{
   // if(m_scene != nullptr)
    if(light != nullptr && light->GetParentNode() != nullptr
     &&GetParentNode() != nullptr)
    {
        if(light->GetType() == OmniLight
            && (GetShadowCastingType() == DynamicShadow || GetShadowCastingType() == AllShadows))
        //if(light->GetDirection().z < 0)
        {
            /*sf::Vector3f lightDirection = Normalize(light->GetDirection());

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
                height_pixel  =  (color_pixel.r + color_pixel.g + color_pixel.b);
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
                        }
                    }
                }
            }

            sf::Texture* shadowTexture = &m_shadowMap[light];
            shadowTexture->create(shadow_bounds.width,shadow_bounds.height);
            shadowTexture->update(shadow_map_array,shadow_bounds.width,shadow_bounds.height,0,0);
            TextureModifier::BlurTexture(shadowTexture, 5);

            std::map<Light*, sf::Drawable*>::iterator shadowIt;
            shadowIt = m_shadowDrawable.find(light);
            if(shadowIt != m_shadowDrawable.end())
                delete m_shadowDrawable[light];

            sf::Sprite *sprite = new sf::Sprite();

            sprite->setTexture(*shadowTexture);
            sprite->setOrigin(sf::Sprite::getOrigin()
                                            -sf::Vector2f(shadow_bounds.left, shadow_bounds.top));

            m_shadowDrawable[light] = sprite;


            delete[] shadow_map_array;*/

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

    /** NEED TO REMOVE THIS AND USE NOTIFICATION SYSTEM**/
    AskForDynamicShadowUpdate();
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
