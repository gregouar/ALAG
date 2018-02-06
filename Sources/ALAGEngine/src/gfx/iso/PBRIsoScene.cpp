#include "ALAGE/gfx/iso/PBRIsoScene.h"


#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/core/Config.h"

#include "ALAGE/gfx/iso/IsoSpriteEntity.h"

#include "ALAGE/core/AssetHandler.h"
//#include "../src/gfx/iso/PBRIsoShaders.cpp"

namespace alag
{

const IsoViewAngle PBRIsoScene::DEFAULT_ISO_VIEW_ANGLE = {.xyAngle = 0,
                                                             .zAngle = 90};

const int PBRIsoScene::MAX_SHADOW_MAPS = 8;


const std::string PBRIsoScene::DEFAULT_ENABLESSAO = "true";
const std::string PBRIsoScene::DEFAULT_ENABLESRGB = "true";
const std::string PBRIsoScene::DEFAULT_SUPERSAMPLING = "1";
const std::string PBRIsoScene::DEFAULT_DIRECTIONALSHADOWSCASTING = "true";
const std::string PBRIsoScene::DEFAULT_DYNAMICSHADOWSCASTING = "true";

PBRIsoScene::PBRIsoScene() : PBRIsoScene(DEFAULT_ISO_VIEW_ANGLE)
{
    //ctor
}

PBRIsoScene::PBRIsoScene(IsoViewAngle viewAngle)
{
    SetViewAngle(viewAngle);

    /*m_colorShader.loadFromMemory(color_fragShader,sf::Shader::Fragment);
    m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
    m_normalShader.loadFromMemory(normal_fragShader,sf::Shader::Fragment);*/
    CompileDepthShader();
    CompileSSAOShader();
    CompileLightingShader();
    CompilePBRGeometryShader();

    //m_PBRGeometryShader.loadFromMemory(PBRGeometry_vertShader,PBRGeometry_fragShader);

    SetAmbientLight(m_ambientLight);
}


PBRIsoScene::~PBRIsoScene()
{
    //dtor
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

    /*if(!m_colorScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_depthScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_normalScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;*/
    if(!m_SSAOScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;


    if(!m_alpha_PBRScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;

    m_alpha_PBRScreen.addRenderTarget(PBRNormalScreen);
    m_alpha_PBRScreen.addRenderTarget(PBRDepthScreen);
    m_alpha_PBRScreen.addRenderTarget(PBRMaterialScreen);

    if(!m_PBRScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;


    m_PBRScreen.addRenderTarget(PBRNormalScreen);
    m_PBRScreen.addRenderTarget(PBRDepthScreen);
    m_PBRScreen.addRenderTarget(PBRMaterialScreen);




    m_renderer.setSize(sf::Vector2f(windowSize.x,windowSize.y));
    m_renderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));
    m_renderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));

    m_lightingShader.setUniform("map_albedo",*m_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_PBRScreen.getTexture(PBRMaterialScreen));
    m_lightingShader.setUniform("map_depthTester",*m_PBRScreen.getTexture(PBRDepthScreen));

    m_lightingShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));


    TextureAsset *brdf_lut = AssetHandler<TextureAsset>::Instance()->LoadAssetFromFile("../data/ibl_brdf_lut.png");
    m_lightingShader.setUniform("map_brdflut",*brdf_lut->GetTexture());

    m_rendererStates.shader = &m_lightingShader;


    //m_lightingShader.setUniform("enable_sRGB",Config::GetBool("graphics","sRGB","true"));
    if(Config::GetBool("graphics","sRGB",DEFAULT_ENABLESRGB))
        EnableGammaCorrection();
    else
        DisableGammaCorrection();

    SetSSAO(Config::GetBool("graphics","SSAO",DEFAULT_ENABLESSAO));

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
        c.r = 0;
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

    std::multimap<float, Light*> lightList;
    m_currentCamera->GetParentNode()->FindNearbyLights(&lightList);

    UpdateLighting(lightList);

    if(m_shadowCastingOption != NoShadow)
        RenderShadows(lightList,curView/*,m_colorScreen.getSize()*/);

    m_alpha_PBRScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        //m_PBRScreen.clear();
        m_alpha_PBRScreen.clear(sf::Color(0,0,0,0));
        m_alpha_PBRScreen.setView(curView);
    m_alpha_PBRScreen.setActive(false);

    m_PBRScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        //m_PBRScreen.clear();
        m_PBRScreen.clear();
        m_PBRScreen.setView(curView);
    m_PBRScreen.setActive(false);



    std::list<SceneEntity*>::iterator renderIt;
    sf::MultipleRenderTexture *renderTarget = nullptr;

    for(int pass = 0 ; pass <= 1 ; ++pass)
    {
        if(pass == 0)
        {
            m_PBRGeometryShader.setUniform("p_alpha_pass",false);
            renderTarget = &m_PBRScreen;
        } else if(pass == 1) {
            m_PBRGeometryShader.setUniform("p_alpha_pass",true);
            renderTarget = &m_alpha_PBRScreen;
        }

        sf::RenderStates state;
        state.shader = &m_PBRGeometryShader;

        renderTarget->setActive(true);
        for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
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
            //m_PBRGeometryShader.setUniform("enable_depthMap",false);
            //m_PBRGeometryShader.setUniform("enable_normalMap",false);
            m_PBRGeometryShader.setUniform("p_normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
            (*renderIt)->PrepareShader(&m_PBRGeometryShader);
            (*renderIt)->Render(renderTarget,state);
        }
        renderTarget->display();
        renderTarget->setActive(false);
    }

    if(m_enableSSAO)
    {
        m_SSAOShader.setUniform("view_zoom",1.0f/m_currentCamera->GetZoom());
        m_SSAOScreen.draw(m_SSAOrenderer,&m_SSAOShader);
        m_SSAOScreen.display();
    }

    sf::Vector2f shift = curView.getCenter();
    shift -= sf::Vector2f(curView.getSize().x/2, curView.getSize().y/2);
    m_lightingShader.setUniform("view_shift",shift);
    m_lightingShader.setUniform("view_zoom",m_currentCamera->GetZoom());
    m_lightingShader.setUniform("view_pos",m_currentCamera->GetParentNode()->GetGlobalPosition());

    m_PBRScreen.setActive(true);

    m_PBRScreen.getTexture(PBRAlbedoScreen)->setSmooth(true);
    m_PBRScreen.getTexture(PBRAlbedoScreen)->generateMipmap();

    //m_PBRScreen.getTexture(PBRNormalScreen)->copyToImage().saveToFile("PBR1.png");

    m_lightingShader.setUniform("map_albedo",*m_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_PBRScreen.getTexture(PBRMaterialScreen));
    m_lightingShader.setUniform("enable_SSAO",m_enableSSAO);
    m_lightingShader.setUniform("enable_depthTesting",false);

    w->draw(m_renderer,m_rendererStates);

    m_alpha_PBRScreen.setActive(true);
    m_lightingShader.setUniform("map_albedo",*m_alpha_PBRScreen.getTexture(PBRAlbedoScreen));
    m_lightingShader.setUniform("map_normal",*m_alpha_PBRScreen.getTexture(PBRNormalScreen));
    m_lightingShader.setUniform("map_depth",*m_alpha_PBRScreen.getTexture(PBRDepthScreen));
    m_lightingShader.setUniform("map_material",*m_alpha_PBRScreen.getTexture(PBRMaterialScreen));
    m_lightingShader.setUniform("enable_SSAO",false);
    m_lightingShader.setUniform("enable_depthTesting",true);

    w->draw(m_renderer,m_rendererStates);

    m_PBRScreen.setActive(false);
    m_alpha_PBRScreen.setActive(false);
}

void PBRIsoScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr && m_currentCamera != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        w->setView(w->getDefaultView());
        ProcessRenderQueue(w);
        w->setView(oldView);
    }
}


int PBRIsoScene::UpdateLighting(std::multimap<float, Light*> &lightList)
{
    int nbr_lights = DefaultScene::UpdateLighting(lightList);

    m_lightingShader.setUniform("light_nbr",(int)nbr_lights);

    int curNbrLights = 0, curNbrShadows = 0;

    float shadowCastingLights[MAX_SHADOW_MAPS] = {-1,-1,-1,-1,-1,-1,-1,-1};
    sf::Vector2f shadowShift[MAX_SHADOW_MAPS];
    sf::Vector2f shadowRatio[MAX_SHADOW_MAPS];

    std::multimap<float, Light*>::iterator lightIt;
    for(lightIt = lightList.begin() ; lightIt != lightList.end()
    && curNbrLights < nbr_lights && curNbrShadows < MAX_SHADOW_MAPS ; ++lightIt)
    {
        std::ostringstream buffer;
        Light* curLight = lightIt->second;

        if(curLight->IsCastShadowEnabled())
        {
            buffer <<"shadow_map_"<<curNbrShadows;
            shadowCastingLights[curNbrShadows] = curNbrLights;
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

    m_lightingShader.setUniformArray("shadow_casters",shadowCastingLights, MAX_SHADOW_MAPS);
    m_lightingShader.setUniformArray("shadow_shift",shadowShift, MAX_SHADOW_MAPS);
    m_lightingShader.setUniformArray("shadow_ratio",shadowRatio, MAX_SHADOW_MAPS);

    return nbr_lights;
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



void PBRIsoScene::SetViewAngle(IsoViewAngle viewAngle)
{
    m_viewAngle = viewAngle;
    ComputeTrigonometry();
}

void PBRIsoScene::SetAmbientLight(sf::Color light)
{
    DefaultScene::SetAmbientLight(light);

    m_lightingShader.setUniform("light_ambient",sf::Glsl::Vec4(m_ambientLight));
    m_lightingShader.setUniform("p_exposure",.8f);
}

void PBRIsoScene::SetSSAO(bool ssao)
{
    m_enableSSAO = ssao;

    if(m_enableSSAO)
    {
        m_lightingShader.setUniform("enable_SSAO", true);
        m_lightingShader.setUniform("map_SSAO", m_SSAOScreen.getTexture());
        m_SSAOShader.setUniform("map_normal", *m_PBRScreen.getTexture(PBRNormalScreen));
        m_SSAOShader.setUniform("map_depth", *m_PBRScreen.getTexture(PBRDepthScreen));
        m_SSAOShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_PBRScreen.getSize().x,
                                                            1.0/(float)m_PBRScreen.getSize().y));

        m_SSAOrenderer.setSize(sf::Vector2f(m_PBRScreen.getSize().x,
                                            m_PBRScreen.getSize().y));

        m_SSAOrenderer.setTexture(m_PBRScreen.getTexture(PBRAlbedoScreen));
    } else {
        m_lightingShader.setUniform("enable_SSAO", false);
    }
}

void PBRIsoScene::SetShadowCasting(ShadowCastingType type)
{
    DefaultScene::SetShadowCasting(type);


    if(type == AllShadows || type == DirectionnalShadow)
        m_lightingShader.setUniform("enable_directionalShadows", true);
    else
        m_lightingShader.setUniform("enable_directionalShadows", false);

    if(type == AllShadows || type == DynamicShadow)
        m_lightingShader.setUniform("enable_directionalShadows", true);
    else
        m_lightingShader.setUniform("enable_dynamicShadows", false);
}

void PBRIsoScene::EnableGammaCorrection()
{
    DefaultScene::EnableGammaCorrection();
    m_lightingShader.setUniform("enable_sRGB", true);
}

void PBRIsoScene::DisableGammaCorrection()
{
    DefaultScene::DisableGammaCorrection();
    m_lightingShader.setUniform("enable_sRGB", false);
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

    m_lightingShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    m_lightingShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
   /* m_lightingShader.setUniform("p_isoToCartMat",sf::Glsl::Vec4(m_isoToCartMat.values[0],
                                                                m_isoToCartMat.values[2],
                                                                m_isoToCartMat.values[1],
                                                                m_isoToCartMat.values[3]));*/
    m_lightingShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);


    m_SSAOShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    m_SSAOShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);
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
