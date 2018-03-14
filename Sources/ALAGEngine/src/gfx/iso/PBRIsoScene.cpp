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

const std::string PBRIsoScene::DEFAULT_ENABLEEDGESMOOTHING = "true";
const std::string PBRIsoScene::DEFAULT_ENABLESSAO = "true";
const std::string PBRIsoScene::DEFAULT_ENABLEBLOOM = "true";
const std::string PBRIsoScene::DEFAULT_ENABLESSR = "true";
const std::string PBRIsoScene::DEFAULT_ENABLESRGB = "true";
const std::string PBRIsoScene::DEFAULT_SUPERSAMPLING = "1";
const std::string PBRIsoScene::DEFAULT_SSR_THRESOLD = "0.8";
const std::string PBRIsoScene::DEFAULT_DIRECTIONALSHADOWSCASTING = "true";
const std::string PBRIsoScene::DEFAULT_DYNAMICSHADOWSCASTING = "true";
//const float PBRIsoScene::DEFAULT_BLOOMBLUR = 12.0;
const float PBRIsoScene::BLOOM_BLUR = 15.0;
const float PBRIsoScene::BLOOM_SCREEN_RATIO = 0.5;
const float PBRIsoScene::SSAO_BLUR = 3.0;
const float PBRIsoScene::SSAO_SCREEN_RATIO = 0.5;
const float PBRIsoScene::ENVIRONMENT_BLUR = 10.0;
const float PBRIsoScene::SSAO_STRENGTH = 2.0;
const unsigned int PBRIsoScene::NBR_PARALLAX_STEPS = 5;

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
    CompileDepthCopierShader();

    CompileWaterGeometryShader();

    SetAmbientLight(m_ambientLight);
    SetEnvironmentMap(nullptr);

    //m_screenTiles = nullptr;
    m_firstStaticRender = true;
    m_swapStaticGeometryBuffers = 0;
}


PBRIsoScene::~PBRIsoScene()
{
   // if(m_screenTiles != nullptr)
     //   delete m_screenTiles;
}

bool CreatePBRScreen(sf::MultipleRenderTexture *buffer, float w, float h, bool useStencil = false)
{
    if(!buffer->create(w, h))
        return false;

    if(useStencil)
        buffer->addDepthStencilBuffer();
    else
        buffer->addDepthBuffer();
    buffer->addRenderTarget(PBRAlbedoScreen);
    buffer->addRenderTarget(PBRNormalScreen);
    buffer->addRenderTarget(PBRDepthScreen);
    buffer->addRenderTarget(PBRMaterialScreen);
    buffer->resetGLStates();

    return true;
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


    m_nbrTiles.x = ceil(windowSize.x/SCREENTILE_SIZE) + 1;
    m_nbrTiles.y = ceil(windowSize.y/SCREENTILE_SIZE) + 1;

    m_screenTiles.resize(m_nbrTiles.x*m_nbrTiles.y);

    std::vector<ScreenTile>::iterator tileIt = m_screenTiles.begin();

    for(size_t y = 0 ; y < m_nbrTiles.y ; ++y)
    for(size_t x = 0 ; x < m_nbrTiles.x ; ++x)
    {
        tileIt->askForUpdate = true;
        tileIt->position = sf::Vector2u(x*SCREENTILE_SIZE,
                                        y*SCREENTILE_SIZE);
        ++tileIt;
    }
    m_tilesShift = sf::Vector2f(0,0);

    if(!CreatePBRScreen(&m_PBRScreen, windowSize.x*m_superSampling, windowSize.y*m_superSampling))
        r = false;

    if(!CreatePBRScreen(&m_alpha_PBRScreen, windowSize.x*m_superSampling, windowSize.y*m_superSampling))
        r = false;

    m_renderer.setSize(sf::Vector2f(windowSize.x,windowSize.y));
    m_renderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));

    m_bloomRect.setSize(sf::Vector2f(windowSize.x*BLOOM_SCREEN_RATIO,
                                     windowSize.y*BLOOM_SCREEN_RATIO));
    m_bloomRect.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling*BLOOM_SCREEN_RATIO,
                                               windowSize.y*m_superSampling*BLOOM_SCREEN_RATIO));

    m_SSAORect.setSize(sf::Vector2f(windowSize.x*SSAO_SCREEN_RATIO,
                                     windowSize.y*SSAO_SCREEN_RATIO));
    m_SSAORect.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling*SSAO_SCREEN_RATIO,
                                               windowSize.y*m_superSampling*SSAO_SCREEN_RATIO));

    if(!m_lighting_PBRScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling))
        r = false;

    m_lighting_PBRScreen.addRenderTarget(0,true);

    for(int i = 0 ; i < 2 ; ++i)
    {
        CreatePBRScreen(&m_staticGeometryScreen[i], m_nbrTiles.x*SCREENTILE_SIZE,
                        m_nbrTiles.y*SCREENTILE_SIZE, true);

        m_staticGeometryScreen[i].setActive(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glEnable(GL_STENCIL_TEST);
        //glStencilFunc(GL_EQUAL, 1, 1);
        glStencilMask(1);

        CreatePBRScreen(&m_alpha_staticGeometryScreen[i], m_nbrTiles.x*SCREENTILE_SIZE,
                        m_nbrTiles.y*SCREENTILE_SIZE, true);

        m_alpha_staticGeometryScreen[i].setActive(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 1);

        if(!m_SSAOScreen[i].create(windowSize.x*m_superSampling*SSAO_SCREEN_RATIO,
                                   windowSize.y*m_superSampling*SSAO_SCREEN_RATIO, true))
            r = false;
        m_SSAOScreen[i].setSmooth(true);

        if(!m_bloomScreen[i].create(windowSize.x*m_superSampling*BLOOM_SCREEN_RATIO,
                                    windowSize.y*m_superSampling*BLOOM_SCREEN_RATIO, false, true))
       // if(!m_bloomScreen[i].create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, false, true))
            r = false;
        m_bloomScreen[i].setSmooth(true);

        if(!m_environment_PBRScreen[i].create(windowSize.x*m_superSampling,
                                              windowSize.y*m_superSampling,false,true))
            r = false;
    }

    m_lightingShader.setUniform("map_SSRenv",m_environment_PBRScreen[0].getTexture());
    m_environment_PBRScreen[0].setSmooth(true);


    m_depthCopierShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));

    m_lightingShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));


    GenerateBrdflut();
    m_lightingShader.setUniform("map_brdflut",m_brdf_lut);

    m_rendererStates.shader = &m_lightingShader;

    if(Config::GetBool("graphics","sRGB",DEFAULT_ENABLESRGB))
        EnableGammaCorrection();
    else
        DisableGammaCorrection();

    SetEdgeSmoothing(Config::GetBool("graphics","EdgeSmoothing",DEFAULT_ENABLEEDGESMOOTHING));
    SetSSAO(Config::GetBool("graphics","SSAO",DEFAULT_ENABLESSAO));
    SetBloom(Config::GetBool("graphics","Bloom",DEFAULT_ENABLEBLOOM));
    SetSSR(Config::GetBool("graphics","SSR",DEFAULT_ENABLESSR),
           Config::GetFloat("graphics","SSR_Thresold",DEFAULT_SSR_THRESOLD));

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
        float theta = (float)RandomNumber(1000)/1000.0;

        sf::Color c = sf::Color::White;
        c.r = 127+cos(theta*2*PI)*256;
        c.g = 127+sin(theta*2*PI)*256;
        c.b = 255;
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

void PBRIsoScene::ProcessRenderQueue()
{
    sf::View curView = GenerateView(m_currentCamera);

    sf::Vector2f shift = curView.getCenter();
    shift -= sf::Vector2f(curView.getSize().x/2, curView.getSize().y/2);
    m_lightingShader.setUniform("view_shift",shift);
    m_lightingShader.setUniform("view_zoom",m_currentCamera->GetZoom());
    m_lightingShader.setUniform("view_pos",m_currentCamera->GetParentNode()->GetGlobalPosition());

    Profiler::PushClock("Render environment map");
        RenderEnvironmentMap();
    Profiler::PopClock();

    Profiler::PushClock("Update lighting");

        Profiler::PushClock("Find lights");
        std::multimap<float, Light*> lightList;
        m_currentCamera->GetParentNode()->FindNearbyLights(&lightList);
        Profiler::PopClock();

        Profiler::PushClock("Compute lights");
        m_lighting_PBRScreen.setActive(true);
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
        ProcessRenderQueue();

        m_renderer.setTexture(m_lighting_PBRScreen.getTexture(0));
        w->draw(m_renderer, &m_HDRBloomShader);

        m_lighting_PBRScreen.display(DOFLUSH);

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
            //shadowRatio[curNbrShadows] = curLight->GetShadowMapRatio();
            shadowRatio[curNbrShadows].x = 1.0/(m_PBRScreen.getSize().x*m_currentCamera->GetZoom() + cur_shift.width);
            shadowRatio[curNbrShadows].y = 1.0/(m_PBRScreen.getSize().y*m_currentCamera->GetZoom() + cur_shift.height);
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

void PBRIsoScene::RenderEnvironmentMap()
{
    m_renderer.setTexture(m_lighting_PBRScreen.getTexture(0));
    m_blurShader.setUniform("offset",sf::Vector2f(0.5*ENVIRONMENT_BLUR/(float)m_lighting_PBRScreen.getSize().x,0));
    m_environment_PBRScreen[1].draw(m_renderer,&m_blurShader);
    m_environment_PBRScreen[1].display(DOFLUSH);

    m_renderer.setTexture(&m_environment_PBRScreen[1].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,0.5*ENVIRONMENT_BLUR/(float)m_lighting_PBRScreen.getSize().x));
    m_environment_PBRScreen[0].draw(m_renderer,&m_blurShader);
    m_environment_PBRScreen[0].display(DOFLUSH);

    m_environment_PBRScreen[0].generateMipmap();
}


void PBRIsoScene::RenderStaticGeometry(const sf::View &curView)
{
    std::list<ScreenTile*> tilesToRender;
    std::list<SceneEntity*> entitiesToRender;

    Profiler::PushClock("Prepare static geometry");

    if(curView.getSize() != m_lastStaticRenderView.getSize())
    {
        m_tilesShift = sf::Vector2f(0,0);
        m_firstStaticRender = true;
    }

    sf::Vector2f zoom(curView.getSize().x/m_PBRScreen.getSize().x,
                      curView.getSize().y/m_PBRScreen.getSize().y);

    if(!m_firstStaticRender)
  //      m_tilesShift += (curView.getCenter() - m_lastStaticRenderView.getCenter());
    {
        m_tilesShift.x += (curView.getCenter().x - m_lastStaticRenderView.getCenter().x)/zoom.x;
        m_tilesShift.y += (curView.getCenter().y - m_lastStaticRenderView.getCenter().y)/zoom.y;
    }

    m_lightingShader.setUniform("SSR_map_shift",
                                sf::Vector2f(curView.getCenter().x - m_lastStaticRenderView.getCenter().x,
                                             m_lastStaticRenderView.getCenter().y - curView.getCenter().y));

    if(m_tilesShift.x < 0 || m_tilesShift.x >= SCREENTILE_SIZE
    || m_tilesShift.y < 0 || m_tilesShift.y >= SCREENTILE_SIZE)
    {
        sf::Vector2i shifting;
        shifting.x = m_tilesShift.x/SCREENTILE_SIZE;
        shifting.y = m_tilesShift.y/SCREENTILE_SIZE;

        if(m_tilesShift.x < 0)
            shifting.x--;
        if(m_tilesShift.y < 0)
            shifting.y--;

        if(shifting.x < (int)m_nbrTiles.x && shifting.x > -(int)m_nbrTiles.x
        && shifting.y < (int)m_nbrTiles.y && shifting.y > -(int)m_nbrTiles.y)
        {
            sf::FloatRect sourceRect, targetRect;

            if(shifting.x > 0)
            {
                sourceRect.left = shifting.x * SCREENTILE_SIZE;
                sourceRect.width = (m_nbrTiles.x - shifting.x) * SCREENTILE_SIZE;
                targetRect.left = 0;
                targetRect.width = sourceRect.width;
            }
            else
            {
                sourceRect.left = 0;
                sourceRect.width = (m_nbrTiles.x + shifting.x) * SCREENTILE_SIZE;
                targetRect.left = -shifting.x * SCREENTILE_SIZE;
                targetRect.width = sourceRect.width;
            }

            if(shifting.y > 0)
            {
                sourceRect.top = shifting.y * SCREENTILE_SIZE;
                sourceRect.height = (m_nbrTiles.y - shifting.y) * SCREENTILE_SIZE;
                targetRect.top = 0;
                targetRect.height = sourceRect.height;
            }
            else
            {
                sourceRect.top = 0;
                sourceRect.height = (m_nbrTiles.y + shifting.y) * SCREENTILE_SIZE;
                targetRect.top = -shifting.y * SCREENTILE_SIZE;
                targetRect.height = sourceRect.height;
            }


            /*CopyPBRScreen(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],sourceRect,
                          &m_staticGeometryScreen[!m_swapStaticGeometryBuffers],targetRect);
            CopyPBRScreen(&m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],sourceRect,
                          &m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers],targetRect);*/

            m_staticGeometryScreen[m_swapStaticGeometryBuffers].setActive(true);

            m_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyDepthBuffer(
                                    &m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    sourceRect, targetRect);
            m_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRAlbedoScreen,sourceRect,
                                    PBRAlbedoScreen, targetRect);
            m_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRDepthScreen,sourceRect,
                                    PBRDepthScreen, targetRect);
            m_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRMaterialScreen,sourceRect,
                                    PBRMaterialScreen, targetRect);
            m_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRNormalScreen,sourceRect,
                                    PBRNormalScreen, targetRect);

            m_staticGeometryScreen[m_swapStaticGeometryBuffers].setActive(false);

            m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyDepthBuffer(
                                    &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    sourceRect, targetRect);
            m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRAlbedoScreen,sourceRect,
                                    PBRAlbedoScreen, targetRect);
            m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRDepthScreen,sourceRect,
                                    PBRDepthScreen, targetRect);
            m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRMaterialScreen,sourceRect,
                                    PBRMaterialScreen, targetRect);
            m_alpha_staticGeometryScreen[!m_swapStaticGeometryBuffers].copyBuffer(
                                    &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers],
                                    PBRNormalScreen,sourceRect,
                                    PBRNormalScreen, targetRect);

            m_swapStaticGeometryBuffers = !m_swapStaticGeometryBuffers;

            for(size_t y = 0 ; y < m_nbrTiles.y ; ++y)
            for(size_t x = 0 ; x < m_nbrTiles.x ; ++x)
            {
                int truex = x, truey = y;

                if(shifting.x < 0)
                    truex = m_nbrTiles.x - x - 1;
                if(shifting.y < 0)
                    truey = m_nbrTiles.y - y - 1;

                if(truex+shifting.x >= 0 && truex+shifting.x < (int)m_nbrTiles.x
                && truey+shifting.y >= 0 && truey+shifting.y < (int)m_nbrTiles.y)
                    m_screenTiles[truex+truey*m_nbrTiles.x].entities =
                        m_screenTiles[(truex+shifting.x) + (truey+shifting.y)*m_nbrTiles.x].entities;
                else
                    m_screenTiles[truex+truey*m_nbrTiles.x].askForUpdate = true;
            }

            m_tilesShift.x -= shifting.x*SCREENTILE_SIZE;
            m_tilesShift.y -= shifting.y*SCREENTILE_SIZE;
        }
        else
            m_tilesShift = sf::Vector2f(0,0); //We need to update to all scene
    }

    sf::Vector2f viewPos;
    viewPos.x = curView.getCenter().x - curView.getSize().x/2 - m_tilesShift.x*zoom.x;
    viewPos.y = curView.getCenter().y - curView.getSize().y/2 - m_tilesShift.y*zoom.y;


    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); )
    {
        if((*renderIt)->IsStatic())
        {
            SceneNode *node = (*renderIt)->GetParentNode();
            if(node != nullptr)
            {
                sf::Vector3f globalPos = m_isoToCartMat*node->GetGlobalPosition();

                sf::FloatRect rect  = (*renderIt)->GetScreenBoundingRect(m_isoToCartMat);
                rect.left += globalPos.x - viewPos.x;
                rect.top  += globalPos.y - viewPos.y;


                for(int  x = std::max(0, (int)floor(rect.left/(SCREENTILE_SIZE*zoom.x))) ;
                            x < std::min((int)m_nbrTiles.x, (int)ceil((float)(rect.left+rect.width)/(SCREENTILE_SIZE*zoom.x))) ;
                            ++x)
                for(int  y = std::max(0, (int)floor(rect.top/(SCREENTILE_SIZE*zoom.y))) ;
                            y < std::min((int)m_nbrTiles.y, (int)ceil((float)(rect.top+rect.height)/(SCREENTILE_SIZE*zoom.y)));
                            ++y)
                {
                    int n = x+y*m_nbrTiles.x;
                    if((*renderIt)->IsAskingForRenderUpdate())
                        m_screenTiles[n].askForUpdate = true;
                    m_screenTiles[n].newList.push_back(*renderIt);
                }
            }

            renderIt = m_renderQueue.erase(renderIt);
        } else
            ++renderIt;
    }


    std::vector<ScreenTile>::iterator tileIt;

    /*for(size_t x = 0 ; x < m_nbrTiles.x ; ++x)
    for(size_t y = 0 ; y < m_nbrTiles.y ; ++y)*/
    for(tileIt = m_screenTiles.begin() ; tileIt != m_screenTiles.end() ; ++tileIt)
    {
        if(!tileIt->askForUpdate && !m_firstStaticRender)
        if(tileIt->newList != tileIt->entities)
            tileIt->askForUpdate = true;

        if(tileIt->askForUpdate || m_firstStaticRender)
        {
            tileIt->entities.clear();
            tileIt->entities = tileIt->newList;
            tileIt->newList = std::list<SceneEntity*>();

            tilesToRender.push_back(&(*tileIt));
            entitiesToRender.insert(entitiesToRender.end(),
                                    tileIt->entities.begin(),
                                    tileIt->entities.end());
            entitiesToRender.sort();
            entitiesToRender.unique();
        }

        tileIt->newList.clear();
    }
    Profiler::PopClock();

    sf::View tileScreenView = curView;

    for(int pass = 0 ; pass <= 1 ; ++pass)
    {
        sf::MultipleRenderTexture *renderTarget = nullptr;

        if(pass == 0)
            renderTarget = &m_staticGeometryScreen[m_swapStaticGeometryBuffers];
        if(pass == 1)
            renderTarget = &m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers];

        renderTarget->setActive(true);
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);

        std::list<ScreenTile*>::iterator tilesIt;
        for(tilesIt = tilesToRender.begin() ; tilesIt != tilesToRender.end() ; ++tilesIt)
        {
            (*tilesIt)->askForUpdate = false;

            renderTarget->setActive(true);
            glScissor((*tilesIt)->position.x,renderTarget->getSize().y-(*tilesIt)->position.y-SCREENTILE_SIZE,
                      SCREENTILE_SIZE,SCREENTILE_SIZE);
            glEnable(GL_SCISSOR_TEST);
            glClearStencil(1);
            if(pass == 0)
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            else if(pass == 1)
            {
                glClear(GL_STENCIL_BUFFER_BIT);
                sf::FloatRect rect = sf::FloatRect((*tilesIt)->position.x, (*tilesIt)->position.y,
                                                    SCREENTILE_SIZE, SCREENTILE_SIZE);
                m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers]
                    .copyDepthBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],rect,rect);
            }
            renderTarget->clear(sf::Color(0,0,0,0));
            glDisable(GL_SCISSOR_TEST);
        //}
        }

        sf::RenderStates state;
        state.shader = &m_PBRGeometryShader;

        tileScreenView = curView;
        tileScreenView.setSize(m_staticGeometryScreen[m_swapStaticGeometryBuffers].getSize().x*zoom.x,
                               m_staticGeometryScreen[m_swapStaticGeometryBuffers].getSize().y*zoom.y);
        tileScreenView.move((-m_tilesShift.x*zoom.x+SCREENTILE_SIZE/2*zoom.x),
                            (-m_tilesShift.y*zoom.y+SCREENTILE_SIZE/2*zoom.y));
        //tileScreenView.setCenter(viewPos.x + tileScreenView.getSize().x/2,
          //                       viewPos.y + tileScreenView.getSize().y/2);

         if(pass == 0)
        {
            m_PBRGeometryShader.setUniform("p_alpha_pass",false);
            //m_PBRGeometryShader.setUniform("enable_depthTesting",false);
            state.blendMode = sf::BlendNone;
        } else if(pass == 1) {
            m_PBRGeometryShader.setUniform("p_alpha_pass",true);
            m_PBRGeometryShader.setUniform("map_opaqueGeometry",
                                           *m_staticGeometryScreen[m_swapStaticGeometryBuffers].getTexture(PBRDepthScreen));
            m_PBRGeometryShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_staticGeometryScreen[m_swapStaticGeometryBuffers].getSize().x,
                                                            1.0/(float)m_staticGeometryScreen[m_swapStaticGeometryBuffers].getSize().y));
            //m_PBRGeometryShader.setUniform("enable_depthTesting",false);
            state.blendMode = sf::BlendNone;
        }

        renderTarget->setView(tileScreenView);

        for(renderIt = entitiesToRender.begin() ; renderIt != entitiesToRender.end() ; ++renderIt)
            RenderEntity(renderTarget,*renderIt);
        /*{
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
        }*/
        renderTarget->display(DOFLUSH);
        renderTarget->setActive(false);
    }

    m_lastStaticRenderView = curView;
    m_firstStaticRender = false;

  /*  m_staticGeometryScreen[m_swapStaticGeometryBuffers].setActive(true);
    m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers].getTexture(PBRDepthScreen)->copyToImage().saveToFile("static0.png");
    //m_alpha_staticGeometryScreen.getTexture(1)->copyToImage().saveToFile("static1.png");
    //m_alpha_staticGeometryScreen.getTexture(2)->copyToImage().saveToFile("static2.png");
    //m_alpha_staticGeometryScreen.getTexture(3)->copyToImage().saveToFile("static3.png");
    m_staticGeometryScreen[m_swapStaticGeometryBuffers].setActive(false);*/
}

void PBRIsoScene::RenderDynamicGeometry(const sf::View &curView)
{
    std::list<SceneEntity*>::iterator renderIt;
    sf::MultipleRenderTexture *renderTarget = nullptr;

    for(int pass = 0 ; pass <= 1 ; ++pass)
    {
       // sf::RenderStates state;
       // state.shader = &m_PBRGeometryShader;


        sf::FloatRect sourceRect = sf::FloatRect(m_tilesShift.x,m_tilesShift.y,
                                                 m_PBRScreen.getSize().x,m_PBRScreen.getSize().y);
        sf::FloatRect targetRect = sf::FloatRect(0,0,m_PBRScreen.getSize().x,
                                                 m_PBRScreen.getSize().y);

        if(pass == 0)
        {
            Profiler::PushClock("Render opaque geometry");
            m_PBRGeometryShader.setUniform("p_alpha_pass",false);
           // m_PBRGeometryShader.setUniform("enable_depthTesting",false);
            renderTarget = &m_PBRScreen;
           // state.blendMode = sf::BlendNone;

           m_PBRScreen.setActive(true);

          /*  m_PBRScreen.copyDepthBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers], sourceRect, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],PBRAlbedoScreen,sourceRect,
                                   PBRAlbedoScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],PBRDepthScreen,sourceRect,
                                   PBRDepthScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],PBRMaterialScreen,sourceRect,
                                   PBRMaterialScreen, targetRect);
            m_PBRScreen.copyBuffer(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],PBRNormalScreen,sourceRect,
                                   PBRNormalScreen, targetRect);*/

            CopyPBRScreen(&m_staticGeometryScreen[m_swapStaticGeometryBuffers],
                          sourceRect, &m_PBRScreen, targetRect);

            //m_PBRScreen.setActive(true);
            //m_PBRScreen.getTexture(0)->copyToImage().saveToFile("statiTest.png");

        } else if(pass == 1) {
            Profiler::PushClock("Render alpha geometry");
            m_PBRGeometryShader.setUniform("p_alpha_pass",true);
            m_PBRGeometryShader.setUniform("map_opaqueGeometry", *m_PBRScreen.getTexture(PBRDepthScreen));
            m_PBRGeometryShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));
            //m_PBRGeometryShader.setUniform("enable_depthTesting",true);
            renderTarget = &m_alpha_PBRScreen;
           // state.blendMode = sf::BlendNone;

            //m_alpha_PBRScreen.setActive(true);

            CopyPBRScreen(&m_alpha_staticGeometryScreen[m_swapStaticGeometryBuffers], sourceRect,
                          &m_alpha_PBRScreen, targetRect, m_PBRScreen.getTexture(PBRDepthScreen));

           /* m_alpha_PBRScreen.setActive(true);
           // m_alpha_PBRScreen.clear(sf::Color(0,0,0,0));
            //glClear(GL_DEPTH_BUFFER_BIT);
           // m_alpha_PBRScreen.copyDepthBuffer(&m_alpha_staticGeometryScreen, sourceRect, targetRect);

            glDepthFunc(GL_ALWAYS);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            m_depthCopierShader.setUniform("map_albedo",*m_alpha_staticGeometryScreen.getTexture(PBRAlbedoScreen));
            m_depthCopierShader.setUniform("map_normal",*m_alpha_staticGeometryScreen.getTexture(PBRNormalScreen));
            m_depthCopierShader.setUniform("map_depth",*m_alpha_staticGeometryScreen.getTexture(PBRDepthScreen));
            m_depthCopierShader.setUniform("map_material",*m_alpha_staticGeometryScreen.getTexture(PBRMaterialScreen));
            m_depthCopierShader.setUniform("map_depthTester",*m_PBRScreen.getTexture(PBRDepthScreen));
            m_depthCopierShader.setUniform("enable_depthTesting",true);
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(m_alpha_PBRScreen.getSize().x, m_alpha_PBRScreen.getSize().y));

            sf::IntRect textRect = sf::IntRect(m_tilesShift.x,m_tilesShift.y,
                                                 m_PBRScreen.getSize().x,m_PBRScreen.getSize().y);

            rect.setPosition(0,0);
            rect.setTexture(m_alpha_staticGeometryScreen.getTexture(PBRAlbedoScreen));
            rect.setTextureRect(textRect);

            sf::RenderStates s;
            s.blendMode = sf::BlendNone;
            s.shader = &m_depthCopierShader;
            m_alpha_PBRScreen.setView(m_alpha_PBRScreen.getDefaultView());
            m_alpha_PBRScreen.draw(rect,s);*/
        }

        renderTarget->setActive(true);

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        renderTarget->setView(curView);

        for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    //    if(!(*renderIt)->IsStatic())
            RenderEntity(renderTarget,*renderIt);

        renderTarget->display(DOFLUSH);
       // renderTarget->setActive(false);
        Profiler::PopClock();
    }

  //  m_PBRScreen.getTexture(PBRDepthScreen)->copyToImage().saveToFile("PBR2.png");
}

void PBRIsoScene::RenderLighting()
{
    m_lighting_PBRScreen.setActive(true);
    m_lighting_PBRScreen.clear();
   // glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);

    m_rendererStates.blendMode = sf::BlendNone;

    m_renderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));
    //m_PBRScreen.getTexture(PBRAlbedoScreen)->copyToImage().saveToFile("PBR0.png");
    //m_PBRScreen.getTexture(PBRNormalScreen)->copyToImage().saveToFile("PBR1.png");

    m_lightingShader.setUniform("map_albedo",*m_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_PBRScreen.getTexture(PBRMaterialScreen));
   // m_lightingShader.setUniform("enable_SSR",m_enableSSR);

    m_lighting_PBRScreen.draw(m_renderer,m_rendererStates);

    //m_lighting_PBRScreen.getTexture(1)->copyToImage().saveToFile("uv.png");
    /*if(m_enableSSAO)
    {
        //m_SSAOScreen[0].getTexture().copyToImage().saveToFile("SSAO.png");
        m_renderer.setTexture(&m_SSAOScreen[0].getTexture());
        m_lighting_PBRScreen.draw(m_renderer,sf::BlendMultiply);
    }*/

   // m_alpha_PBRScreen.getTexture(PBRAlbedoScreen)->copyToImage().saveToFile("aPBR0.png");
   // m_alpha_PBRScreen.getTexture(PBRDepthScreen)->copyToImage().saveToFile("aPBR1.png");


    m_rendererStates.blendMode = sf::BlendAlpha;

    m_lightingShader.setUniform("map_albedo",*m_alpha_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_alpha_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_alpha_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_alpha_PBRScreen.getTexture(PBRMaterialScreen));
    //m_lightingShader.setUniform("enable_SSR",false);
    //m_lightingShader.setUniform("enable_SSR",m_enableSSR);

    m_lighting_PBRScreen.draw(m_renderer,m_rendererStates);




    m_lighting_PBRScreen.display(DOFLUSH);
}

void PBRIsoScene::RenderBloom()
{
   // m_lighting_PBRScreen[m_activeLightingPBRScreen].getTexture(1)->copyToImage().saveToFile("bloom.png");
    m_bloomScreen[0].clear();
    m_bloomScreen[1].clear();

    sf::RectangleShape screenToBloom;
    screenToBloom.setTexture(m_lighting_PBRScreen.getTexture(1));
    screenToBloom.setSize(sf::Vector2f(m_bloomScreen[0].getSize()));
    m_blurShader.setUniform("offset",sf::Vector2f(BLOOM_BLUR/(float)m_PBRScreen.getSize().x,0));
    m_bloomScreen[0].draw(screenToBloom,&m_blurShader);
    m_bloomScreen[0].display(DOFLUSH);
    m_bloomRect.setTexture(&m_bloomScreen[0].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,BLOOM_BLUR/(float)m_PBRScreen.getSize().y));
    m_bloomScreen[1].draw(m_bloomRect,&m_blurShader);
    m_bloomScreen[1].display(DOFLUSH);

    for(int i = 0 ; i < 2 ; ++i) {
        m_bloomRect.setTexture(&m_bloomScreen[1].getTexture());
        m_blurShader.setUniform("offset",sf::Vector2f(BLOOM_BLUR/(float)m_PBRScreen.getSize().x/(i+2),0));
        m_bloomScreen[0].draw(m_bloomRect,&m_blurShader);
        m_bloomScreen[0].display(DOFLUSH);
        m_bloomRect.setTexture(&m_bloomScreen[0].getTexture());
        m_blurShader.setUniform("offset",sf::Vector2f(0,BLOOM_BLUR/(float)m_PBRScreen.getSize().y/(i+2)));
        m_bloomScreen[1].draw(m_bloomRect,&m_blurShader);
        m_bloomScreen[1].display(DOFLUSH);
    }
   // m_bloomScreen[1].getTexture().copyToImage().saveToFile("Bloom.png");
}

void PBRIsoScene::RenderSSAO()
{
    m_SSAOShader.setUniform("view_zoom",1.0f/m_currentCamera->GetZoom());

    m_SSAOScreen[0].draw(m_SSAOrenderer,&m_SSAOShader);
    m_SSAOScreen[0].display(DOFLUSH);

    m_SSAORect.setTexture(&m_SSAOScreen[0].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(SSAO_BLUR*SSAO_SCREEN_RATIO/(float)m_SSAOScreen[0].getSize().x,0));
    m_SSAOScreen[1].draw(m_SSAORect,&m_blurShader);
    m_SSAOScreen[1].display(DOFLUSH);
    m_SSAORect.setTexture(&m_SSAOScreen[1].getTexture());
    m_blurShader.setUniform("offset",sf::Vector2f(0,SSAO_BLUR*SSAO_SCREEN_RATIO/(float)m_SSAOScreen[0].getSize().x));
    m_SSAOScreen[0].draw(m_SSAORect,&m_blurShader);
    m_SSAOScreen[0].display(DOFLUSH);

    //m_SSAOScreen[0].getTexture().copyToImage().saveToFile("SSAO.png");
}

void PBRIsoScene::RenderEntity(sf::RenderTarget* renderTarget,SceneEntity *entity)
{
    if(entity != nullptr)
    {
        sf::RenderStates state;
        state.shader = &m_PBRGeometryShader;
        state.blendMode = sf::BlendNone;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = entity->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        sf::Vector3f v = m_isoToCartMat*globalPos;

        state.transform = sf::Transform::Identity;
        state.transform.translate(v.x, v.y);
        state.transform *= m_TransformIsoToCart;

        /** I should probably add resetShader something**/
        m_PBRGeometryShader.setUniform("enable_parallax",false);
        m_PBRGeometryShader.setUniform("enable_volumetricOpacity",false);
        m_PBRGeometryShader.setUniform("enable_foamCollision",false);
        m_PBRGeometryShader.setUniform("p_zPos",globalPos.z*PBRTextureAsset::DEPTH_BUFFER_NORMALISER);
        m_PBRGeometryShader.setUniform("p_normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
        entity->PrepareShader(&m_PBRGeometryShader);
        entity->Render(renderTarget,state);
    }
}

IsoRectEntity* PBRIsoScene::CreateIsoRectEntity(sf::Vector2f rectSize)
{
    IsoRectEntity *e = new IsoRectEntity(rectSize);
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoScene(this);
    return e;
}

IsoWaterEntity* PBRIsoScene::CreateIsoWaterEntity(sf::Vector2f rectSize)
{
    IsoWaterEntity *e = new IsoWaterEntity(rectSize);
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoScene(this);
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

void PBRIsoScene::SetEnvironmentMap(TextureAsset* env_map)
{
    /** Should generate filtering **/
    if(env_map != nullptr)
    {
        env_map->GenerateMipmap();
        env_map->SetSmooth(true);
        m_lightingShader.setUniform("map_environmental",*env_map->GetTexture());
        m_lightingShader.setUniform("enable_map_environmental",true);
    } else
        m_lightingShader.setUniform("enable_map_environmental",false);

}

void PBRIsoScene::SetEdgeSmoothing(bool es)
{
    m_PBRGeometryShader.setUniform("enable_edgeSmoothing",es);
}

void PBRIsoScene::SetSSAO(bool ssao)
{
    m_enableSSAO = ssao;

    if(m_enableSSAO)
    {
        m_lightingShader.setUniform("enable_SSAO", true);
        //m_HDRBloomShader.setUniform("enable_SSAO", true);
        m_lightingShader.setUniform("map_SSAO", m_SSAOScreen[0].getTexture());
       // m_HDRBloomShader.setUniform("map_SSAO", m_SSAOScreen[0].getTexture());
        m_SSAOShader.setUniform("map_normal", *m_PBRScreen.getTexture(PBRNormalScreen));
        m_SSAOShader.setUniform("map_depth", *m_PBRScreen.getTexture(PBRDepthScreen));
        m_SSAOShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));

        m_SSAOrenderer.setSize(sf::Vector2f(m_SSAOScreen[0].getSize()));
        m_SSAOrenderer.setTextureRect(sf::IntRect(0,0,m_PBRScreen.getSize().x,
                                                  m_PBRScreen.getSize().y));


        m_SSAOrenderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));
    } else {
        m_lightingShader.setUniform("enable_SSAO", false);
        //m_HDRBloomShader.setUniform("enable_SSAO", false);
    }
}

void PBRIsoScene::SetBloom(bool bloom)
{
    if(bloom && !m_enableBloom)
    {
        m_lighting_PBRScreen.addRenderTarget(1,true);
        m_lighting_PBRScreen.getTexture(1)->setSmooth(true);
        m_lightingShader.setUniform("enable_bloom", true);
        m_HDRBloomShader.setUniform("enable_bloom", true);
    } else if(!bloom && m_enableBloom) {
        m_lighting_PBRScreen.removeRenderTarget(1);
        m_lightingShader.setUniform("enable_bloom", false);
        m_HDRBloomShader.setUniform("enable_bloom", false);
    }

    m_enableBloom = bloom;
}

void PBRIsoScene::SetSSR(bool SSR, float thresold)
{
    if(SSR && !m_enableSSR)
    {
        m_environment_PBRScreen[0].clear();
        m_lightingShader.setUniform("enable_SSR", true);
        if(thresold >= 0)
            m_lightingShader.setUniform("p_SSRThresold", thresold);
    } else if(!SSR && m_enableSSR) {
        m_environment_PBRScreen[1].clear();
        m_lightingShader.setUniform("enable_SSR", false);
    }

    m_enableSSR = SSR;
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


    //CompileLightingShader();

    m_PBRGeometryShader.setUniform("view_direction", m_normalProjMat * sf::Vector3f(0,0,1));
    m_PBRGeometryShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));

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

sf::Shader* PBRIsoScene::GetWaterGeometryShader()
{
    return &m_waterGeometryShader;
}

void PBRIsoScene::CopyPBRScreen(sf::MultipleRenderTexture *source, sf::FloatRect sourceRect,
                    sf::MultipleRenderTexture *target, sf::FloatRect targetRect, sf::Texture *depthTester)
{
    if(source != nullptr && target != nullptr)
    {
        target->setActive(true);

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        m_depthCopierShader.setUniform("map_albedo",*source->getTexture(PBRAlbedoScreen));
        m_depthCopierShader.setUniform("map_normal",*source->getTexture(PBRNormalScreen));
        m_depthCopierShader.setUniform("map_depth",*source->getTexture(PBRDepthScreen));
        m_depthCopierShader.setUniform("map_material",*source->getTexture(PBRMaterialScreen));

        if(depthTester != nullptr)
        {
            m_depthCopierShader.setUniform("map_depthTester",*depthTester);
            m_depthCopierShader.setUniform("enable_depthTesting",true);
        } else {
            m_depthCopierShader.setUniform("enable_depthTesting",false);
        }

        sf::RectangleShape rect;
        rect.setPosition(targetRect.left, targetRect.top);
        rect.setSize(sf::Vector2f(targetRect.width, targetRect.height));

        sf::IntRect textRect = sf::IntRect(sourceRect.left,sourceRect.top,
                                            sourceRect.width,sourceRect.height);

        rect.setTexture(source->getTexture(PBRAlbedoScreen));
        rect.setTextureRect(textRect);

        sf::RenderStates s;
        s.blendMode = sf::BlendNone;
        s.shader = &m_depthCopierShader;

        sf::View oldView = target->getView();
        target->setView(target->getDefaultView());
        target->draw(rect,s);
        target->setView(oldView);

        target->display(DOFLUSH);

        glDepthFunc(GL_LEQUAL);
    }
}

void PBRIsoScene::CopyPBRScreen(sf::MultipleRenderTexture *source,
                                          sf::MultipleRenderTexture *target, sf::Texture *depthTester)
{
    CopyPBRScreen(source, sf::FloatRect(0,0,source->getSize().x,source->getSize().y),
                  target, sf::FloatRect(0,0,target->getSize().x,target->getSize().y), depthTester);
}

}
