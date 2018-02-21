#include "ALAGE/gfx/iso/PBRIsoScene.h"


#include "ALAGE/utils/Profiler.h"
#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/core/Config.h"

#include "ALAGE/gfx/iso/IsoSpriteEntity.h"

#include "ALAGE/core/AssetHandler.h"
//#include "../src/gfx/iso/PBRIsoShaders.cpp"

namespace alag
{

const bool DOFLUSH = false;

const IsoViewAngle PBRIsoScene::DEFAULT_ISO_VIEW_ANGLE = {.xyAngle = 0,
                                                             .zAngle = 90};

const int PBRIsoScene::MAX_SHADOW_MAPS = 8;
const int PBRIsoScene::SCREENTILE_SIZE = 128;

const std::string PBRIsoScene::DEFAULT_ENABLESSAO = "true";
const std::string PBRIsoScene::DEFAULT_ENABLEBLOOM = "true";
const std::string PBRIsoScene::DEFAULT_ENABLESRGB = "true";
const std::string PBRIsoScene::DEFAULT_SUPERSAMPLING = "1";
const std::string PBRIsoScene::DEFAULT_DIRECTIONALSHADOWSCASTING = "true";
const std::string PBRIsoScene::DEFAULT_DYNAMICSHADOWSCASTING = "true";
const float PBRIsoScene::DEFAULT_BLOOMBLUR = 12.0;
const float PBRIsoScene::DEFAULT_SSAOBLUR = 3.0;
const float PBRIsoScene::SSAO_STRENGTH = 3.0;

PBRIsoScene::PBRIsoScene() : PBRIsoScene(DEFAULT_ISO_VIEW_ANGLE)
{
    //ctor
}

PBRIsoScene::PBRIsoScene(IsoViewAngle viewAngle)
{
    SetViewAngle(viewAngle);

    CompileSSAOShader();
    CompileLightingShader();
    CompilePBRGeometryShader();
    CompileBlurShader();
    CompileHDRBloomShader();

    CompileDepthShader();

    SetAmbientLight(m_ambientLight);

    m_screenTiles = nullptr;
}


PBRIsoScene::~PBRIsoScene()
{
    if(m_screenTiles != nullptr)
        delete m_screenTiles;
}

bool PBRIsoScene::InitRenderer(sf::Vector2u windowSize)
{
    bool r = true;


    m_superSampling = Config::GetInt("graphics","SuperSampling",DEFAULT_SUPERSAMPLING);

    bool directShadow = Config::GetBool("graphics","DirectionalShadowsCasting",DEFAULT_DIRECTIONALSHADOWSCASTING);
    bool dynamicShadow = Config::GetBool("graphics","DynamicShadowsCasting",DEFAULT_DYNAMICSHADOWSCASTING);

    if(directShadow)
    {
        if(dynamicShadow)
            SetShadowCasting(AllShadows);
        else
            SetShadowCasting(DirectionnalShadow);
    } else if(dynamicShadow)
        SetShadowCasting(DynamicShadow);

    if(!m_SSAOScreen[0].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_SSAOScreen[1].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;

    m_SSAOScreen[0].setSmooth(true);
    m_SSAOScreen[1].setSmooth(true);




    m_nbrTiles.x = ceil(windowSize.x/SCREENTILE_SIZE) + 1;
    m_nbrTiles.y = ceil(windowSize.y/SCREENTILE_SIZE) + 1;

    if(m_screenTiles != nullptr)
        delete m_screenTiles;

    m_screenTiles = new ScreenTile[m_nbrTiles.x*m_nbrTiles.y];
    for(size_t x = 0 ; x < m_nbrTiles.x ; ++x)
    for(size_t y = 0 ; y < m_nbrTiles.y ; ++y)
    {
        m_screenTiles[x + y*m_nbrTiles.x].askForUpdate = true;
        m_screenTiles[x + y*m_nbrTiles.x].position = sf::Vector2u(x*SCREENTILE_SIZE,
                                                                  y*SCREENTILE_SIZE);
    }
   // m_tilesShift = sf::Vector2f(SCREENTILE_SIZE,SCREENTILE_SIZE);
    m_tilesShift = sf::Vector2f(0,0);

    if(!m_staticGeometryScreen.create( m_nbrTiles.x*SCREENTILE_SIZE,
                                       m_nbrTiles.y*SCREENTILE_SIZE, true))
        r = false;

    m_staticGeometryScreen.addRenderTarget(PBRNormalScreen);
    m_staticGeometryScreen.addRenderTarget(PBRDepthScreen);
    m_staticGeometryScreen.addRenderTarget(PBRMaterialScreen);


    if(!m_alpha_PBRScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;

    if(!m_PBRScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;

    m_PBRScreen.addRenderTarget(PBRNormalScreen);
    m_PBRScreen.addRenderTarget(PBRDepthScreen);
    m_PBRScreen.addRenderTarget(PBRMaterialScreen);

    m_alpha_PBRScreen.addRenderTarget(PBRNormalScreen);
    m_alpha_PBRScreen.addRenderTarget(PBRDepthScreen);
    m_alpha_PBRScreen.addRenderTarget(PBRMaterialScreen);


    if(!m_lighting_PBRScreen[0].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, false, true))
        r = false;
    if(!m_lighting_PBRScreen[1].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, false, true))
        r = false;

    if(!m_bloomScreen[0].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, false, true))
        r = false;
    if(!m_bloomScreen[1].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, false, true))
        r = false;

    m_renderer.setSize(sf::Vector2f(windowSize.x,windowSize.y));
    m_renderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));
    //m_renderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));

    m_lightingShader.setUniform("map_albedo",*m_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_PBRScreen.getTexture(PBRMaterialScreen));

   /* m_PBRGeometryShader.setUniform("map_depthTester",*m_PBRScreen.getTexture(PBRDepthScreen));

    m_PBRGeometryShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));*/

    m_lightingShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));

    //m_HDRBloomShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
    //                                                        1.0/(float)m_PBRScreen.getSize().y));

    TextureAsset *brdf_lut = AssetHandler<TextureAsset>::Instance()->LoadAssetFromFile("../data/ibl_brdf_lut.png");
    m_lightingShader.setUniform("map_brdflut",*brdf_lut->GetTexture());

    m_rendererStates.shader = &m_lightingShader;

    if(Config::GetBool("graphics","sRGB",DEFAULT_ENABLESRGB))
        EnableGammaCorrection();
    else
        DisableGammaCorrection();

    SetSSAO(Config::GetBool("graphics","SSAO",DEFAULT_ENABLESSAO));
    SetBloom(Config::GetBool("graphics","Bloom",DEFAULT_ENABLEBLOOM));

    m_HDRBloomShader.setUniform("bloom_map",m_bloomScreen[1].getTexture());

    sf::Glsl::Vec3 samplesHemisphere[16];
    samplesHemisphere[0] = sf::Glsl::Vec3(.4,0,.8);
    samplesHemisphere[1] = sf::Glsl::Vec3(0,.2,.4);
    samplesHemisphere[2] = sf::Glsl::Vec3(.1,0,.2);
    samplesHemisphere[3] = sf::Glsl::Vec3(0,0,.1);

    samplesHemisphere[4] = sf::Glsl::Vec3(1,0,.4);
    samplesHemisphere[5] = sf::Glsl::Vec3(-1,0,.4);
    samplesHemisphere[6] = sf::Glsl::Vec3(0,1,.4);
    samplesHemisphere[7] = sf::Glsl::Vec3(0,-1,.4);

    samplesHemisphere[8] = sf::Glsl::Vec3(.5,.5,.5);
    samplesHemisphere[9] = sf::Glsl::Vec3(.5,-.5,.5);
    samplesHemisphere[10] = sf::Glsl::Vec3(-.5,.5,.5);
    samplesHemisphere[11] = sf::Glsl::Vec3(-.5,-.5,.5);

    samplesHemisphere[12] = sf::Glsl::Vec3(.5,0,.5);
    samplesHemisphere[13] = sf::Glsl::Vec3(-.5,0,.5);
    samplesHemisphere[14] = sf::Glsl::Vec3(0,.5,.5);
    samplesHemisphere[15] = sf::Glsl::Vec3(0,-.5,.5);

    m_SSAOShader.setUniformArray("p_samplesHemisphere",samplesHemisphere,16);

    m_SSAONoisePattern.create(4,4);

    for(int x = 0 ; x < 4 ; ++x)
    for(int y = 0 ; y < 4 ; ++y)
    {
        sf::Color c = sf::Color::White;
        c.r = (int)(static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/255)));
        c.g = (int)(static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/255)));
        c.b = 1.0;
        m_SSAONoisePattern.setPixel(x,y,c);
    }

    m_SSAONoiseTexture.setRepeated(true);
    m_SSAONoiseTexture.loadFromImage(m_SSAONoisePattern);
    m_SSAONoiseTexture.setRepeated(true);
    m_SSAOShader.setUniform("map_noise",m_SSAONoiseTexture);
    m_lightingShader.setUniform("map_noise",m_SSAONoiseTexture);

    if(!r)
        Logger::Error("Cannot initialize isometric renderer");

    return r;
}

void PBRIsoScene::ProcessRenderQueue(sf::RenderTarget *w)
{
    sf::View curView = GenerateView(m_currentCamera);

    sf::Vector2f shift = curView.getCenter();
    shift -= sf::Vector2f(curView.getSize().x/2, curView.getSize().y/2);
    m_lightingShader.setUniform("view_shift",shift);
    m_lightingShader.setUniform("view_zoom",m_currentCamera->GetZoom());
    m_lightingShader.setUniform("view_pos",m_currentCamera->GetParentNode()->GetGlobalPosition());


    Profiler::PushClock("Update lighting");

        Profiler::PushClock("Find lights");
        std::multimap<float, Light*> lightList;
        m_currentCamera->GetParentNode()->FindNearbyLights(&lightList);
        Profiler::PopClock();

        Profiler::PushClock("Compute lights");
        m_lighting_PBRScreen[m_activeLightingPBRScreen].setActive(true);
        UpdateLighting(lightList);
        Profiler::PopClock();

        Profiler::PushClock("Render shadows");
        if(m_shadowCastingOption != NoShadow)
            RenderShadows(lightList,curView);
        Profiler::PopClock();

    Profiler::PopClock();


    Profiler::PushClock("Render static geometry");
        RenderStaticGeometry(curView);
    Profiler::PopClock();


    Profiler::PushClock("Render dynamic geometry");
        RenderDynamicGeometry(curView);
    Profiler::PopClock();

    if(m_enableSSAO)
    {
        Profiler::PushClock("SSAO");
            RenderSSAO();
        Profiler::PopClock();
    }

    Profiler::PushClock("Render lighting");
        RenderLighting();
    Profiler::PopClock();

    if(m_enableBloom)
    {
        Profiler::PushClock("Bloom");
            RenderBloom();
        Profiler::PopClock();
    }
}

void PBRIsoScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr && m_currentCamera != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        w->setView(w->getDefaultView());
        ProcessRenderQueue(w);


        m_renderer.setTexture(m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(0));
        w->draw(m_renderer, &m_HDRBloomShader);

        m_lighting_PBRScreen[m_activeLightingPBRScreen].display(DOFLUSH);
        m_lightingShader.setUniform("map_environmental",*m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(0));
        m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(0)->setSmooth(true);
        m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(0)->generateMipmap();
        m_activeLightingPBRScreen = !m_activeLightingPBRScreen;


        w->setView(oldView);
    }
}

int PBRIsoScene::UpdateLighting(std::multimap<float, Light*> &lightList)
{
    int nbr_lights = DefaultScene::UpdateLighting(lightList);

    m_lightingShader.setUniform("light_nbr",(int)nbr_lights);

    int curNbrLights = 0, curNbrShadows = 0;

    float shadowCastingLights[MAX_SHADOW_MAPS];

    for(size_t i = 0 ; i <  MAX_SHADOW_MAPS ; ++i)
        shadowCastingLights[i] = -1;

    sf::Vector2f shadowShift[MAX_SHADOW_MAPS];
    sf::Vector2f shadowRatio[MAX_SHADOW_MAPS];

    std::multimap<float, Light*>::iterator lightIt;
    for(lightIt = lightList.begin() ; lightIt != lightList.end()
    && curNbrLights < nbr_lights && curNbrShadows < MAX_SHADOW_MAPS ; ++lightIt)
    {
        Light* curLight = lightIt->second;

        if(curLight->IsCastShadowEnabled())
        {
            std::ostringstream buffer;
            buffer <<"shadow_map_"<<curNbrShadows;
            shadowCastingLights[curNbrShadows] = (float)curNbrLights;
            sf::IntRect cur_shift = curLight->GetShadowMaxShift();
            shadowShift[curNbrShadows] = sf::Vector2f(cur_shift.left,
                                                      -cur_shift.height - cur_shift.top ); /*GLSL Reverse y-coord*/
            shadowRatio[curNbrShadows] = sf::Vector2f(1.0/(float)curLight->GetShadowMap().getSize().x,
                                                      1.0/(float)curLight->GetShadowMap().getSize().y);
            m_lightingShader.setUniform(buffer.str(), curLight->GetShadowMap());

            ++curNbrShadows;
        }
        ++curNbrLights;
    }

    m_lightingShader.setUniformArray("shadow_caster",shadowCastingLights, MAX_SHADOW_MAPS);
    m_lightingShader.setUniformArray("shadow_shift",shadowShift, MAX_SHADOW_MAPS);
    m_lightingShader.setUniformArray("shadow_ratio",shadowRatio, MAX_SHADOW_MAPS);

    return nbr_lights;
}


void PBRIsoScene::RenderStaticGeometry(const sf::View &curView)
{
    std::list<ScreenTile*> tilesToRender;

    sf::Vector2f viewPos;
    viewPos.x = curView.getCenter().x - curView.getSize().x/2 - m_tilesShift.x;
    viewPos.y = curView.getCenter().y - curView.getSize().y/2 - m_tilesShift.y;

    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    if((*renderIt)->IsStatic())
    {
        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
        {
            /**Need to take zoom into consideration somewhere**/
            sf::Vector3f globalPos = m_isoToCartMat*node->GetGlobalPosition();

            sf::FloatRect rect  = (*renderIt)->GetScreenBoundingRect(m_isoToCartMat);
            rect.left += globalPos.x - viewPos.x;
            rect.top += globalPos.y - viewPos.y;

           // std::cout<<rect.left<<" "<<rect.top<<" "<<rect.width<<" "<<rect.height<<std::endl;

            for(int  x = std::max(0, (int)floor(rect.left/SCREENTILE_SIZE)) ;
                        x < std::min((int)m_nbrTiles.x, (int)ceil((float)(rect.left+rect.width)/SCREENTILE_SIZE)) ;
                        ++x)
            for(int  y = std::max(0, (int)floor(rect.top/SCREENTILE_SIZE)) ;
                        y < std::min((int)m_nbrTiles.y, (int)ceil((float)(rect.top+rect.height)/SCREENTILE_SIZE));
                        ++y)
            {
                if((*renderIt)->IsAskingForRenderUpdate())
                    m_screenTiles[x+y*m_nbrTiles.x].askForUpdate = true;
                m_screenTiles[x+y*m_nbrTiles.x].newList.push_back(*renderIt);
            }
        }
    }

    for(size_t x = 0 ; x < m_nbrTiles.x ; ++x)
    for(size_t y = 0 ; y < m_nbrTiles.y ; ++y)
    {
        size_t n = x+y*m_nbrTiles.x;

        /** DO A TRUE COMPARISON HERE **/
        if(m_screenTiles[n].newList.size() !=  m_screenTiles[n].entities.size())
        {
            m_screenTiles[n].askForUpdate = true;
            m_screenTiles[n].entities.clear();
            m_screenTiles[n].entities = m_screenTiles[n].newList;
            m_screenTiles[n].newList = std::list<SceneEntity*>();
        }

        if(m_screenTiles[n].askForUpdate)
            tilesToRender.push_back(&m_screenTiles[n]);

        m_screenTiles[n].newList.clear();
    }


    sf::FloatRect viewport;
    viewport.width = 1.0/m_nbrTiles.x;
    viewport.height = 1.0/m_nbrTiles.y;

    sf::View tileScreenView = curView;
    tileScreenView.setSize(SCREENTILE_SIZE,SCREENTILE_SIZE); /** PUT ZOOM HERE **/


    std::list<ScreenTile*>::iterator tilesIt;
    for(tilesIt = tilesToRender.begin() ; tilesIt != tilesToRender.end() ; ++tilesIt)
    {
        (*tilesIt)->askForUpdate = false;

        viewport.left = (float)(*tilesIt)->position.x/(m_nbrTiles.x*SCREENTILE_SIZE);
        viewport.top = (float)(*tilesIt)->position.y/(m_nbrTiles.y*SCREENTILE_SIZE);
        tileScreenView.setViewport(viewport);
        tileScreenView.setCenter(viewPos.x+(*tilesIt)->position.x+SCREENTILE_SIZE/2,
                                 viewPos.y+(*tilesIt)->position.y+SCREENTILE_SIZE/2); /** ADD ZOOM HERE **/


        std::list<SceneEntity*>::iterator renderIt;
        sf::MultipleRenderTexture *renderTarget = nullptr;

        sf::RenderStates state;
        state.shader = &m_PBRGeometryShader;

        /** Should maybe inverse pass and tiles**/
        //for(int pass = 0 ; pass <= 1 ; ++pass)
        {
            //if(pass == 0)
            {
                m_PBRGeometryShader.setUniform("p_alpha_pass",false);
                renderTarget = &m_staticGeometryScreen;
                state.blendMode = sf::BlendNone;
            }/* else if(pass == 1) {
                Profiler::PushClock("Render alpha geometry");
                m_PBRGeometryShader.setUniform("p_alpha_pass",true);
                renderTarget = &m_alpha_PBRScreen;
                state.blendMode = sf::BlendNone;
                m_alpha_PBRScreen.copyDepthBuffer(&m_PBRScreen);
            }*/

            renderTarget->setActive(true);
            glScissor((*tilesIt)->position.x,renderTarget->getSize().y-(*tilesIt)->position.y-SCREENTILE_SIZE,
                      SCREENTILE_SIZE,SCREENTILE_SIZE);
            glEnable(GL_SCISSOR_TEST);
           // if(pass == 0)
                glClear(GL_DEPTH_BUFFER_BIT);
            renderTarget->clear(sf::Color(0,0,0,0));
            glDisable(GL_SCISSOR_TEST);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            renderTarget->setView(tileScreenView);

            for(renderIt = (*tilesIt)->entities.begin() ;
                renderIt != (*tilesIt)->entities.end();
                ++renderIt)
            {
                sf::Vector3f globalPos(0,0,0);

                SceneNode *node = (*renderIt)->GetParentNode();
                if(node != nullptr)
                    globalPos = node->GetGlobalPosition();

                sf::Vector3f v = m_isoToCartMat*globalPos;

                state.transform = sf::Transform::Identity;
                state.transform.translate(v.x, v.y);
                state.transform *= m_TransformIsoToCart;

                m_PBRGeometryShader.setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
                m_PBRGeometryShader.setUniform("p_normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
                (*renderIt)->PrepareShader(&m_PBRGeometryShader);
                (*renderIt)->Render(renderTarget,state);
                //std::cout<<"test"<<std::endl;
            }
            renderTarget->display(DOFLUSH);
            //renderTarget->setActive(false);
        }
    }

   /* m_staticGeometryScreen.setActive(true);
    m_staticGeometryScreen.getTexture(0)->copyToImage().saveToFile("static0.png");
    m_staticGeometryScreen.getTexture(1)->copyToImage().saveToFile("static1.png");
    m_staticGeometryScreen.getTexture(2)->copyToImage().saveToFile("static2.png");
    m_staticGeometryScreen.getTexture(3)->copyToImage().saveToFile("static3.png");*/
}

void PBRIsoScene::RenderDynamicGeometry(const sf::View &curView)
{
    std::list<SceneEntity*>::iterator renderIt;
    sf::MultipleRenderTexture *renderTarget = nullptr;

    for(int pass = 0 ; pass <= 1 ; ++pass)
    {
        sf::RenderStates state;
        state.shader = &m_PBRGeometryShader;

        if(pass == 0)
        {
            Profiler::PushClock("Render opaque geometry");
            m_PBRGeometryShader.setUniform("p_alpha_pass",false);
            renderTarget = &m_PBRScreen;
            state.blendMode = sf::BlendNone;

            sf::FloatRect sourceRect = sf::FloatRect(m_tilesShift.x,m_tilesShift.y,
                                                     m_PBRScreen.getSize().x,m_PBRScreen.getSize().y);
            sf::FloatRect targetRect = sf::FloatRect(0,0,m_PBRScreen.getSize().x,
                                                     m_PBRScreen.getSize().y);

            m_PBRScreen.copyDepthBuffer(&m_staticGeometryScreen, sourceRect, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen,PBRAlbedoScreen,sourceRect,
                                   PBRAlbedoScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen,PBRDepthScreen,sourceRect,
                                   PBRDepthScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen,PBRMaterialScreen,sourceRect,
                                   PBRMaterialScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen,PBRNormalScreen,sourceRect,
                                   PBRNormalScreen, targetRect);

            //m_PBRScreen.setActive(true);
            //m_PBRScreen.getTexture(0)->copyToImage().saveToFile("statiTest.png");

        } else if(pass == 1) {
            Profiler::PushClock("Render alpha geometry");
            m_PBRGeometryShader.setUniform("p_alpha_pass",true);
            renderTarget = &m_alpha_PBRScreen;
            state.blendMode = sf::BlendNone;
            m_alpha_PBRScreen.copyDepthBuffer(&m_PBRScreen);
        }

        renderTarget->setActive(true);

        if(pass == 0)
        {
            //glClear(GL_DEPTH_BUFFER_BIT);
        } else
            renderTarget->clear(sf::Color(0,0,0,0));

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        renderTarget->setView(curView);

        for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
        if(!(*renderIt)->IsStatic())
        {
            sf::Vector3f globalPos(0,0,0);

            SceneNode *node = (*renderIt)->GetParentNode();
            if(node != nullptr)
                globalPos = node->GetGlobalPosition();

            sf::Vector3f v = m_isoToCartMat*globalPos;

            state.transform = sf::Transform::Identity;
            state.transform.translate(v.x, v.y);
            state.transform *= m_TransformIsoToCart;

            m_PBRGeometryShader.setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
            m_PBRGeometryShader.setUniform("p_normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
            (*renderIt)->PrepareShader(&m_PBRGeometryShader);
            (*renderIt)->Render(renderTarget,state);
        }
        renderTarget->display(DOFLUSH);
        renderTarget->setActive(false);
        Profiler::PopClock();
    }
}

void PBRIsoScene::RenderLighting()
{
    m_renderer.setTexture(m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(0));

    m_lightingShader.setUniform("map_albedo",*m_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_PBRScreen.getTexture(PBRMaterialScreen));

    m_lighting_PBRScreen[m_activeLightingPBRScreen].draw(m_renderer,m_rendererStates);

    m_lightingShader.setUniform("map_albedo",*m_alpha_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_alpha_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_alpha_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_alpha_PBRScreen.getTexture(PBRMaterialScreen));

    m_lighting_PBRScreen[m_activeLightingPBRScreen].draw(m_renderer,m_rendererStates);

    if(m_enableSSAO)
    {
        //m_SSAOScreen[0].getTexture().copyToImage().saveToFile("SSAO.png");
        m_renderer.setTexture(&m_SSAOScreen[0].getTexture());
        m_lighting_PBRScreen[m_activeLightingPBRScreen].draw(m_renderer,sf::BlendMultiply);
    }

    m_lighting_PBRScreen[m_activeLightingPBRScreen].display(DOFLUSH);
}

void PBRIsoScene::RenderBloom()
{
   // m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(1)->copyToImage().saveToFile("bloom.png");
    m_renderer.setTexture(m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(1));
    m_blurShader.setUniform("offset",sf::Vector2f(DEFAULT_BLOOMBLUR/(float)m_PBRScreen.getSize().x,0));
    m_bloomScreen[0].draw(m_renderer,&m_blurShader);
    m_bloomScreen[0].display(DOFLUSH);
    m_renderer.setTexture(&m_bloomScreen[0].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,DEFAULT_BLOOMBLUR/(float)m_PBRScreen.getSize().y));
    m_bloomScreen[1].draw(m_renderer,&m_blurShader);
    m_bloomScreen[1].display(DOFLUSH);

    m_renderer.setTexture(&m_bloomScreen[1].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0.5*DEFAULT_BLOOMBLUR/(float)m_PBRScreen.getSize().x,0));
    m_bloomScreen[0].draw(m_renderer,&m_blurShader);
    m_bloomScreen[0].display(DOFLUSH);
    m_renderer.setTexture(&m_bloomScreen[0].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,0.5*DEFAULT_BLOOMBLUR/(float)m_PBRScreen.getSize().y));
    m_bloomScreen[1].draw(m_renderer,&m_blurShader);
    m_bloomScreen[1].display(DOFLUSH);

   // m_bloomScreen[1].getTexture().copyToImage().saveToFile("Bloom.png");
}

void PBRIsoScene::RenderSSAO()
{
    m_SSAOShader.setUniform("view_zoom",1.0f/m_currentCamera->GetZoom());

    m_SSAOScreen[0].draw(m_SSAOrenderer,&m_SSAOShader);
    m_SSAOScreen[0].display(DOFLUSH);

    m_renderer.setTexture(&m_SSAOScreen[0].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(DEFAULT_SSAOBLUR/(float)m_PBRScreen.getSize().x,0));
    m_SSAOScreen[1].draw(m_renderer,&m_blurShader);
    m_SSAOScreen[1].display(DOFLUSH);
    m_renderer.setTexture(&m_SSAOScreen[1].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,DEFAULT_SSAOBLUR/(float)m_PBRScreen.getSize().x));
    m_SSAOScreen[0].draw(m_renderer,&m_blurShader);
    m_SSAOScreen[0].display(DOFLUSH);
}


IsoRectEntity* PBRIsoScene::CreateIsoRectEntity(sf::Vector2f rectSize)
{
    IsoRectEntity *e = new IsoRectEntity(rectSize);
    AddCreatedObject(GenerateObjectID(), e);
    return e;
}


IsoSpriteEntity* PBRIsoScene::CreateIsoSpriteEntity(sf::Vector2i spriteSize)
{
    return CreateIsoSpriteEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

IsoSpriteEntity* PBRIsoScene::CreateIsoSpriteEntity(sf::IntRect textureRect)
{
    IsoSpriteEntity *e = new IsoSpriteEntity(textureRect);
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoScene(this);
    return e;
}

IsoGeometricShadowCaster* PBRIsoScene::CreateIsoGeometricShadowCaster()
{
    IsoGeometricShadowCaster *e = new IsoGeometricShadowCaster();
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoScene(this);
    return e;
}



void PBRIsoScene::SetViewAngle(IsoViewAngle viewAngle)
{
    m_viewAngle = viewAngle;
    ComputeTrigonometry();
}

void PBRIsoScene::SetAmbientLight(sf::Color light)
{
    DefaultScene::SetAmbientLight(light);

    m_lightingShader.setUniform("light_ambient",sf::Glsl::Vec4(m_ambientLight));
    //m_lightingShader.setUniform("p_exposure",.8f);
    m_HDRBloomShader.setUniform("p_exposure",.8f);
}

void PBRIsoScene::SetSSAO(bool ssao)
{
    m_enableSSAO = ssao;

    if(m_enableSSAO)
    {
        //m_lightingShader.setUniform("enable_SSAO", true);
        //m_HDRBloomShader.setUniform("enable_SSAO", true);
       // m_lightingShader.setUniform("map_SSAO", m_SSAOScreen.getTexture());
       // m_HDRBloomShader.setUniform("map_SSAO", m_SSAOScreen[0].getTexture());
        m_SSAOShader.setUniform("map_normal", *m_PBRScreen.getTexture(PBRNormalScreen));
        m_SSAOShader.setUniform("map_depth", *m_PBRScreen.getTexture(PBRDepthScreen));
        m_SSAOShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));

        m_SSAOrenderer.setSize(sf::Vector2f(m_PBRScreen.getSize().x,
                                            m_PBRScreen.getSize().y));

        m_SSAOrenderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));
    } else {
        //m_lightingShader.setUniform("enable_SSAO", false);
        //m_HDRBloomShader.setUniform("enable_SSAO", false);
    }
}

void PBRIsoScene::SetBloom(bool bloom)
{
    if(bloom && !m_enableBloom)
    {
        m_lighting_PBRScreen[0].addRenderTarget(1,true);
        m_lighting_PBRScreen[1].addRenderTarget(1,true);
        m_lighting_PBRScreen[0].getTexture(1)->setSmooth(true);
        m_lighting_PBRScreen[1].getTexture(1)->setSmooth(true);
        m_lightingShader.setUniform("enable_bloom", true);
        m_HDRBloomShader.setUniform("enable_bloom", true);
    } else if(!bloom && m_enableBloom) {
        m_lighting_PBRScreen[0].removeRenderTarget(1);
        m_lighting_PBRScreen[1].removeRenderTarget(1);
        m_lightingShader.setUniform("enable_bloom", false);
        m_HDRBloomShader.setUniform("enable_bloom", false);
    }

    m_enableBloom = bloom;
}

void PBRIsoScene::SetShadowCasting(ShadowCastingType type)
{
    DefaultScene::SetShadowCasting(type);


    if(type == AllShadows || type == DirectionnalShadow)
        m_lightingShader.setUniform("enable_directionalShadows", true);
    else
        m_lightingShader.setUniform("enable_directionalShadows", false);

    if(type == AllShadows || type == DynamicShadow)
        m_lightingShader.setUniform("enable_dynamicShadows", true);
    else
        m_lightingShader.setUniform("enable_dynamicShadows", false);
}

void PBRIsoScene::EnableGammaCorrection()
{
    DefaultScene::EnableGammaCorrection();
    m_lightingShader.setUniform("enable_sRGB", true);
    m_HDRBloomShader.setUniform("enable_sRGB", true);
}

void PBRIsoScene::DisableGammaCorrection()
{
    DefaultScene::DisableGammaCorrection();
    m_lightingShader.setUniform("enable_sRGB", false);
    m_HDRBloomShader.setUniform("enable_sRGB", false);
}



void PBRIsoScene::ComputeTrigonometry()
{
     float cosXY = cos(m_viewAngle.xyAngle*PI/180.0);
     float sinXY = sin(m_viewAngle.xyAngle*PI/180.0);
     float cosZ = cos(m_viewAngle.zAngle*PI/180);
     float sinZ = sin(m_viewAngle.zAngle*PI/180);

     m_TransformIsoToCart = sf::Transform(cosXY,         -sinXY,         0,
                                         sinXY * sinZ,   cosXY * sinZ, 0,
                                            0, 0, 1);

     m_isoToCartMat = Mat3x3(cosXY        , -sinXY       , 0    ,
                             sinXY * sinZ , cosXY * sinZ , -cosZ,
                             0            , 0            , 0);

     m_isoToCartMat2X2 = Mat2x2(cosXY        , -sinXY       ,
                             sinXY * sinZ , cosXY * sinZ );

     m_cartToIsoMat = Mat3x3( cosXY , sinXY/sinZ, 0,
                             -sinXY , cosXY/sinZ, 0,
                              0     , 0         , 0);

    m_normalProjMat = Mat3x3 ( cosXY ,  sinZ * sinXY , cosZ * sinXY,
                              -sinXY ,  sinZ * cosXY , cosZ * cosXY,
                               0     , -cosZ         , sinZ);

     m_normalProjMatInv = Mat3x3(   cosXY        , -sinXY        , 0,
                                    sinXY*sinZ   ,  cosXY*sinZ   , -cosZ,
                                    sinXY * cosZ ,  cosXY*cosZ   , sinZ);


   // m_lightingShader.setUniform("view_direction",sf::Glsl::Vec3(sf::Vector3f(cosXY*cosZ, sinXY*sinZ,sinZ)));
    m_lightingShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    m_lightingShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
   /* m_lightingShader.setUniform("p_isoToCartMat",sf::Glsl::Vec4(m_isoToCartMat.values[0],
                                                                m_isoToCartMat.values[2],
                                                                m_isoToCartMat.values[1],
                                                                m_isoToCartMat.values[3]));*/
    m_lightingShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);


    //m_SSAOShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    //m_SSAOShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);
    m_SSAOShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
}


Mat3x3 PBRIsoScene::GetIsoToCartMat()
{
    return m_isoToCartMat;
}

Mat3x3 PBRIsoScene::GetCartToIsoMat()
{
    return m_cartToIsoMat;
}

const sf::Transform &PBRIsoScene::GetIsoToCartTransform()
{
    return m_TransformIsoToCart;
}

sf::Vector2f PBRIsoScene::ConvertIsoToCartesian(float x, float y, float z)
{
    return ConvertIsoToCartesian(sf::Vector3f(x,y,z));
}

sf::Vector2f PBRIsoScene::ConvertIsoToCartesian(sf::Vector2f p)
{
    return m_isoToCartMat*p;
}

sf::Vector2f PBRIsoScene::ConvertIsoToCartesian(sf::Vector3f p)
{
    sf::Vector3f r = m_isoToCartMat*p;
    return sf::Vector2f(r.x, r.y);
}


sf::Vector2f PBRIsoScene::ConvertCartesianToIso(float x, float y)
{
    return ConvertCartesianToIso(sf::Vector2f(x,y));
}

sf::Vector2f PBRIsoScene::ConvertCartesianToIso(sf::Vector2f p)
{
    return m_cartToIsoMat*p;
}

sf::Vector2f PBRIsoScene::ConvertMouseToScene(sf::Vector2i mouse)
{
    sf::Vector2f scenePos = sf::Vector2f(mouse);
    if(m_last_target != nullptr && m_currentCamera != nullptr)
    {
        sf::View oldView = m_last_target->getView();
        m_last_target->setView(GenerateView(m_currentCamera));
        scenePos = sf::Vector2f(m_last_target->mapPixelToCoords(mouse));
        scenePos = ConvertCartesianToIso(scenePos);
        m_last_target->setView(oldView);
    }
    return scenePos;
}

sf::View PBRIsoScene::GenerateView(Camera* cam)
{
    sf::View v;
    if(cam != nullptr)
    {
        v.setSize(cam->GetSize()*cam->GetZoom());
        SceneNode *node = cam->GetParentNode();
        if(node != nullptr)
            v.setCenter(ConvertIsoToCartesian(node->GetGlobalPosition()));
    }
    return v;
}

sf::Shader* PBRIsoScene::GetDepthShader()
{
    return &m_depthShader;
}

}
