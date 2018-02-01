#include "ALAGE/gfx/iso/IsometricScene.h"


#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/core/Config.h"

#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "../src/gfx/iso/IsometricShaders.cpp"

namespace alag
{

//const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE.xyAngle = 0;
//const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE.zAngle = 90;

const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE = {.xyAngle = 0,
                                                             .zAngle = 90};


IsometricScene::IsometricScene() : IsometricScene(DEFAULT_ISO_VIEW_ANGLE)
{
    //ctor
}

IsometricScene::IsometricScene(IsoViewAngle viewAngle)
{
    SetViewAngle(viewAngle);

    m_colorShader.loadFromMemory(color_fragShader,sf::Shader::Fragment);
    m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
    m_normalShader.loadFromMemory(normal_fragShader,sf::Shader::Fragment);
    m_SSAOShader.loadFromMemory(vertexShader,SSAO_fragShader);
    m_lightingShader.loadFromMemory(vertexShader,lighting_fragShader);
    SetAmbientLight(m_ambientLight);
}


IsometricScene::~IsometricScene()
{
    //dtor
}

bool IsometricScene::InitRenderer(sf::Vector2u windowSize)
{
    bool r = true;

    m_superSampling = Config::GetInt("graphics","SuperSampling","1");

    if(!m_colorScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_depthScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_normalScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;
    if(!m_SSAOScreen.create(windowSize.x*m_superSampling, windowSize.y*m_superSampling, true))
        r = false;

    m_colorScreen.setActive(true);
        m_colorScreen.setSmooth(true);
    m_colorScreen.setActive(false);

    m_depthScreen.setActive(true);
        m_depthScreen.setSmooth(true);
    m_depthScreen.setActive(false);

    m_normalScreen.setActive(true);
        m_normalScreen.setSmooth(true);
    m_normalScreen.setActive(false);


    m_renderer.setSize(sf::Vector2f(windowSize.x,windowSize.y));
    m_renderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));
    m_renderer.setTexture(&m_colorScreen.getTexture());

    m_lightingShader.setUniform("map_color",m_colorScreen.getTexture());
    m_lightingShader.setUniform("map_normal",m_normalScreen.getTexture());
    m_lightingShader.setUniform("map_depth",m_depthScreen.getTexture());
    m_lightingShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_colorScreen.getSize().x,
                                                            1.0/(float)m_colorScreen.getSize().y));

    m_rendererStates.shader = &m_lightingShader;


    SetSSAO(Config::GetBool("graphics","SSAO","true"));

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
        c.r = 1;
        m_SSAONoisePattern.setPixel(x,y,c);
    }

    m_SSAONoiseTexture.setRepeated(true);
    m_SSAONoiseTexture.loadFromImage(m_SSAONoisePattern);
    m_SSAONoiseTexture.setRepeated(true);
    m_SSAOShader.setUniform("map_noise",m_SSAONoiseTexture);

    if(!r)
        Logger::Error("Cannot initialize isometric renderer");

    return r;
}

void IsometricScene::ProcessRenderQueue(sf::RenderTarget *w)
{
    std::multimap<float, Light*> lightList;
    m_currentCamera->GetParentNode()->FindNearbyLights(&lightList);
  //  m_rootNode.FindNearbyLights(&lightList);
    UpdateLighting(lightList);

    sf::View curView = GenerateView(m_currentCamera);

    RenderShadows(lightList,curView,m_colorScreen.getSize());

    m_colorScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_colorScreen.clear();
        m_colorScreen.setView(curView);
    m_colorScreen.setActive(false);

    m_depthScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_depthScreen.clear(sf::Color::White);
        m_depthScreen.setView(curView);
    m_depthScreen.setActive(false);

    m_normalScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_normalScreen.clear();
        m_normalScreen.setView(curView);
    m_normalScreen.setActive(false);


    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::RenderStates state;
        state.transform = sf::Transform::Identity;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        /*state.transform.translate(ConvertIsoToCartesian(0,0,globalPos.z));
        state.transform *= m_TransformIsoToCart;
        state.transform.translate(globalPos.x, globalPos.y);*/

        sf::Vector3f v = m_isoToCartMat*globalPos;

        state.transform.translate(v.x, v.y);
        state.transform *= m_TransformIsoToCart;

        m_colorScreen.setActive(true);
            m_colorShader.setUniform("p_zPos",globalPos.z);
            (*renderIt)->PrepareShader(&m_colorShader);
            state.shader = &m_colorShader;
            (*renderIt)->Render(&m_colorScreen,state);
        m_colorScreen.setActive(false);

        m_normalScreen.setActive(true);
            m_normalShader.setUniform("p_zPos",globalPos.z);
            m_normalShader.setUniform("enable_normalMap",false);
            m_normalShader.setUniform("p_normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
            m_normalShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
            m_normalShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);
            (*renderIt)->PrepareShader(&m_normalShader);
            state.shader = &m_normalShader;
            (*renderIt)->Render(&m_normalScreen,state);
        m_normalScreen.setActive(false);

        m_depthScreen.setActive(true);
            m_depthShader.setUniform("p_zPos",globalPos.z);
            (*renderIt)->PrepareShader(&m_depthShader);
            state.shader = &m_depthShader;
            (*renderIt)->Render(&m_depthScreen,state);
        m_depthScreen.setActive(false);

    }

    m_colorScreen.display();
    m_depthScreen.display();
    m_normalScreen.display();

    /*m_colorScreen.getTexture().copyToImage().saveToFile("color.png");
    m_depthScreen.getTexture().copyToImage().saveToFile("depth.png");
    m_normalScreen.getTexture().copyToImage().saveToFile("normal.png");*/

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
    w->draw(m_renderer,m_rendererStates);
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr && m_currentCamera != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        w->setView(w->getDefaultView());
        ProcessRenderQueue(w);
        w->setView(oldView);


        /*sf::View oldView = w->getView();
        glClear(GL_DEPTH_BUFFER_BIT);
        w->setView(GenerateView(m_currentCamera));
        ProcessRenderQueue(w);

        if(m_enableSSAO)
        {
            m_geometryScreen[!m_useSecondScreen].display();
            m_useSecondScreen = !m_useSecondScreen;
        }
        w->setView(oldView);*/
    }
}


int IsometricScene::UpdateLighting(std::multimap<float, Light*> &lightList)
{
    int nbr_lights = SceneManager::UpdateLighting(lightList);

    m_lightingShader.setUniform("light_nbr",(int)nbr_lights);

    int curNbrLights = 0, curNbrShadows = 0;

    float shadowCastingLights[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
    sf::Vector2f shadowShift[8];
    sf::Vector2f shadowRatio[8];

    std::multimap<float, Light*>::iterator lightIt;
    for(lightIt = lightList.begin() ; lightIt != lightList.end()
    && curNbrLights < nbr_lights && curNbrShadows < 8 ; ++lightIt) /** REPLACE 8 BY CONSTANT**/
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

    m_lightingShader.setUniformArray("shadow_casters",shadowCastingLights, 8);
    m_lightingShader.setUniformArray("shadow_shift",shadowShift, 8);
    m_lightingShader.setUniformArray("shadow_ratio",shadowRatio, 8);

    return nbr_lights;
}



IsoRectEntity* IsometricScene::CreateIsoRectEntity(sf::Vector2f rectSize)
{
    IsoRectEntity *e = new IsoRectEntity(rectSize);
    AddCreatedObject(GenerateObjectID(), e);
    return e;
}


IsoSpriteEntity* IsometricScene::CreateIsoSpriteEntity(sf::Vector2i spriteSize)
{
    return CreateIsoSpriteEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

IsoSpriteEntity* IsometricScene::CreateIsoSpriteEntity(sf::IntRect textureRect)
{
    IsoSpriteEntity *e = new IsoSpriteEntity(textureRect);
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoScene(this);
    return e;
}



void IsometricScene::SetViewAngle(IsoViewAngle viewAngle)
{
    m_viewAngle = viewAngle;
    ComputeTrigonometry();
}

void IsometricScene::SetAmbientLight(sf::Color light)
{
    SceneManager::SetAmbientLight(light);

    m_lightingShader.setUniform("light_ambient",sf::Glsl::Vec4(m_ambientLight));
}

void IsometricScene::SetSSAO(bool ssao)
{
    m_enableSSAO = ssao;

    if(m_enableSSAO)
    {
        m_lightingShader.setUniform("enable_SSAO", true);
        m_lightingShader.setUniform("map_SSAO", m_SSAOScreen.getTexture());
        m_SSAOShader.setUniform("map_normal", m_normalScreen.getTexture());
        m_SSAOShader.setUniform("map_depth", m_depthScreen.getTexture());
        m_SSAOShader.setUniform("view_ratio",sf::Vector2f(1.0/(float)m_depthScreen.getSize().x,
                                                            1.0/(float)m_depthScreen.getSize().y));

        m_SSAOrenderer.setSize(sf::Vector2f(m_depthScreen.getSize().x,
                                            m_depthScreen.getSize().y));
       // m_SSAOrenderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));
        m_SSAOrenderer.setTexture(&m_colorScreen.getTexture());
    } else {
        m_lightingShader.setUniform("enable_SSAO", false);
    }
}

void IsometricScene::ComputeTrigonometry()
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

     m_cartToIsoMat = Mat3x3( cosXY , sinXY/sinZ, 0,
                             -sinXY , cosXY/sinZ, 0,
                              0     , 0         , 0);

     m_normalProjMat.values[0] = cosXY;
     m_normalProjMat.values[1] = sinZ * sinXY;
     m_normalProjMat.values[2] = cosZ * sinXY;
     m_normalProjMat.values[3] = -sinXY;
     m_normalProjMat.values[4] = sinZ * cosXY;
     m_normalProjMat.values[5] = cosZ * cosXY;
     m_normalProjMat.values[6] = 0;
     m_normalProjMat.values[7] = -cosZ;
     m_normalProjMat.values[8] = sinZ;


     m_normalProjMatInv.values[0] = cosXY;
     m_normalProjMatInv.values[1] = -sinXY;
     m_normalProjMatInv.values[2] = 0;
     m_normalProjMatInv.values[3] = sinXY*sinZ;
     m_normalProjMatInv.values[4] = cosXY*sinZ;
     m_normalProjMatInv.values[5] = -cosZ;
     m_normalProjMatInv.values[6] = sinXY * cosZ;
     m_normalProjMatInv.values[7] = cosXY*cosZ;
     m_normalProjMatInv.values[8] = sinZ;


    m_lightingShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    m_lightingShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
    m_lightingShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);


    m_SSAOShader.setUniform("p_cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIsoMat.values));
    m_SSAOShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);
    m_SSAOShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
}


Mat3x3 IsometricScene::GetIsoToCartMat()
{
    return m_isoToCartMat;
}

Mat3x3 IsometricScene::GetCartToIsoMat()
{
    return m_cartToIsoMat;
}

sf::Vector2f IsometricScene::ConvertIsoToCartesian(float x, float y, float z)
{
    return ConvertIsoToCartesian(sf::Vector3f(x,y,z));
}

sf::Vector2f IsometricScene::ConvertIsoToCartesian(sf::Vector2f p)
{
    return m_isoToCartMat*p;
}

sf::Vector2f IsometricScene::ConvertIsoToCartesian(sf::Vector3f p)
{
    sf::Vector3f r = m_isoToCartMat*p;
    return sf::Vector2f(r.x, r.y);
}


sf::Vector2f IsometricScene::ConvertCartesianToIso(float x, float y)
{
    return ConvertCartesianToIso(sf::Vector2f(x,y));
}

sf::Vector2f IsometricScene::ConvertCartesianToIso(sf::Vector2f p)
{
    return m_cartToIsoMat*p;
}

sf::Vector2f IsometricScene::ConvertMouseToScene(sf::Vector2i mouse)
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

sf::View IsometricScene::GenerateView(Camera* cam)
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

sf::Shader* IsometricScene::GetDepthShader()
{
    return &m_depthShader;
}

}
