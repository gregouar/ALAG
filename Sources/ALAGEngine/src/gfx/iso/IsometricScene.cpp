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
    //m_depthAndLightingShader.loadFromMemory(depthAndLighting_fragShader,sf::Shader::Fragment);
    //m_depthAndLightingShader.loadFromMemory(vertexShader,depthAndLighting_fragShader);
    m_lightingShader.loadFromMemory(vertexShader,lighting_fragShader);
   // m_geometryShader.loadFromMemory(vertexShader,geometry_fragShader);
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

    m_lightingShader.setUniform("colorMap",m_colorScreen.getTexture());
    m_lightingShader.setUniform("normalMap",m_normalScreen.getTexture());
    m_lightingShader.setUniform("depthMap",m_depthScreen.getTexture());
    m_lightingShader.setUniform("screen_ratio",sf::Vector2f(1.0/(float)m_colorScreen.getSize().x,
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

    m_SSAOShader.setUniformArray("samplesHemisphere",samplesHemisphere,16);

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
    m_SSAOShader.setUniform("noiseMap",m_SSAONoiseTexture);

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
        m_depthScreen.clear();
        m_depthScreen.setView(curView);
    m_depthScreen.setActive(false);

    m_normalScreen.setActive(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_normalScreen.clear();
        m_normalScreen.setView(curView);
    m_normalScreen.setActive(false);

    /*w->pushGLStates();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    if(m_enableSSAO)
    {
        m_geometryScreen[!m_useSecondScreen].setActive(true);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_geometryScreen[!m_useSecondScreen].clear();
        glClear(GL_DEPTH_BUFFER_BIT);
        m_geometryScreen[!m_useSecondScreen].setView(w->getView());
        m_geometryScreen[!m_useSecondScreen].setActive(false);
    }

    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::RenderStates state;
        state.transform = sf::Transform::Identity;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        state.transform.translate(ConvertIsoToCartesian(0,0,globalPos.z));
        state.transform *= m_TransformIsoToCart;
        state.transform.translate(globalPos.x, globalPos.y);

        sf::Shader* curShader = nullptr;

        if((*renderIt)->CanBeLighted())
            curShader = &m_depthAndLightingShader;
        else
            curShader = &m_depthShader;

        if(curShader != nullptr)
        {
            curShader->setUniform("zPos",globalPos.z);
            curShader->setUniform("useNormalMap",true);
            curShader->setUniform("normalProjMat",sf::Glsl::Mat3(m_normalProjMat));

            curShader->setUniform("cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIso2DProjMat));
            curShader->setUniform("isoToCartZFactor",m_isoToCartZFactor);

            if(m_enableSSAO)
            {
                curShader->setUniform("useSSAO",true);
                curShader->setUniform("geometryMap",m_geometryScreen[m_useSecondScreen].getTexture());
                curShader->setUniform("screen_ratio",sf::Vector2f(1.0/(float)m_geometryScreen[m_useSecondScreen].getSize().x,
                                                                  1.0/(float)m_geometryScreen[m_useSecondScreen].getSize().y));
            } else {
                curShader->setUniform("useSSAO",false);
            }

        }

        (*renderIt)->PrepareShader(curShader);
        state.shader = curShader;
        (*renderIt)->Render(w,state);


        if(m_enableSSAO)
        {
            m_geometryScreen[!m_useSecondScreen].setActive(true);
            curShader = &m_geometryShader;
            {
                curShader->setUniform("zPos",globalPos.z);
                curShader->setUniform("useNormalMap",true);
                curShader->setUniform("normalProjMat",sf::Glsl::Mat3(m_normalProjMat));
                curShader->setUniform("cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIso2DProjMat));
                curShader->setUniform("isoToCartZFactor",m_isoToCartZFactor);
                curShader->setUniform("isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat));
                curShader->setUniform("geometryMap",m_geometryScreen[m_useSecondScreen].getTexture());
                curShader->setUniform("screen_ratio",sf::Vector2f(1.0/(float)m_geometryScreen[m_useSecondScreen].getSize().x,
                                                                  1.0/(float)m_geometryScreen[m_useSecondScreen].getSize().y));
            }
            state.shader = curShader;
            (*renderIt)->PrepareShader(curShader);
            (*renderIt)->Render(&m_geometryScreen[!m_useSecondScreen], state);
            m_geometryScreen[!m_useSecondScreen].setActive(false);
        }
    }
    w->popGLStates();*/

    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::RenderStates state;
        state.transform = sf::Transform::Identity;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        state.transform.translate(ConvertIsoToCartesian(0,0,globalPos.z));
        state.transform *= m_TransformIsoToCart;
        state.transform.translate(globalPos.x, globalPos.y);


        m_colorScreen.setActive(true);
            m_colorShader.setUniform("zPos",globalPos.z);
            (*renderIt)->PrepareShader(&m_colorShader);
            state.shader = &m_colorShader;
            (*renderIt)->Render(&m_colorScreen,state);
        m_colorScreen.setActive(false);

        m_normalScreen.setActive(true);
            m_normalShader.setUniform("zPos",globalPos.z);
            m_normalShader.setUniform("useNormalMap",false);
            m_normalShader.setUniform("normalProjMat",sf::Glsl::Mat3(m_normalProjMat.values));
            m_normalShader.setUniform("cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIso2DProjMat.values));
            m_normalShader.setUniform("isoToCartZFactor",-m_isoToCartMat.values[5]);
            (*renderIt)->PrepareShader(&m_normalShader);
            state.shader = &m_normalShader;
            (*renderIt)->Render(&m_normalScreen,state);
        m_normalScreen.setActive(false);

        m_depthScreen.setActive(true);
            m_depthShader.setUniform("zPos",globalPos.z);
            (*renderIt)->PrepareShader(&m_depthShader);
            state.shader = &m_depthShader;
            (*renderIt)->Render(&m_depthScreen,state);
        m_depthScreen.setActive(false);

    }

    m_colorScreen.display();
    m_depthScreen.display();
    m_normalScreen.display();

    if(m_enableSSAO)
    {
        m_SSAOShader.setUniform("zoom",1.0f/m_currentCamera->GetZoom());
        m_SSAOScreen.draw(m_SSAOrenderer,&m_SSAOShader);
        m_SSAOScreen.display();
    }

    sf::Vector2f decal = curView.getCenter();
    decal -= sf::Vector2f(curView.getSize().x/2, curView.getSize().y/2);
    m_lightingShader.setUniform("view_decal",decal);
    m_lightingShader.setUniform("zoom",m_currentCamera->GetZoom());
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

int IsometricScene::UpdateLighting(std::multimap<float, Light*> &ligtList)
{
    int nbr_lights = SceneManager::UpdateLighting(ligtList);

    m_lightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);
   // m_depthAndLightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);

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

    m_lightingShader.setUniform("ambient_light",sf::Glsl::Vec4(m_ambientLight));
   // m_depthAndLightingShader.setUniform("ambient_light",sf::Glsl::Vec4(m_ambientLight));
}

void IsometricScene::SetSSAO(bool ssao)
{
    m_enableSSAO = ssao;

    if(m_enableSSAO)
    {
        m_lightingShader.setUniform("useSSAO", true);
        m_lightingShader.setUniform("SSAOMap", m_SSAOScreen.getTexture());
        m_SSAOShader.setUniform("normalMap", m_normalScreen.getTexture());
        m_SSAOShader.setUniform("depthMap", m_depthScreen.getTexture());
        m_SSAOShader.setUniform("screen_ratio",sf::Vector2f(1.0/(float)m_depthScreen.getSize().x,
                                                            1.0/(float)m_depthScreen.getSize().y));

        m_SSAOrenderer.setSize(sf::Vector2f(m_depthScreen.getSize().x,
                                            m_depthScreen.getSize().y));
       // m_SSAOrenderer.setTextureRect(sf::IntRect(0,0,windowSize.x*m_superSampling, windowSize.y*m_superSampling));
        m_SSAOrenderer.setTexture(&m_colorScreen.getTexture());
    } else {
        m_lightingShader.setUniform("useSSAO", false);
    }
}

void IsometricScene::ComputeTrigonometry()
{
    float cosXY = cos(m_viewAngle.xyAngle*PI/180.0);
    float sinXY = sin(m_viewAngle.xyAngle*PI/180.0);
    float cosZ = cos(m_viewAngle.zAngle*PI/180);
    float sinZ = sin(m_viewAngle.zAngle*PI/180);

    m_IsoToCart_xVector.x =  cosXY;
    m_IsoToCart_xVector.y =  sinXY * sinZ;

    m_IsoToCart_yVector.x = -sinXY;
    m_IsoToCart_yVector.y =  cosXY * sinZ;

    m_IsoToCart_zVector.x =  0;
    m_IsoToCart_zVector.y = -cosZ;

    m_CartToIso_xVector.x =  cosXY;
    m_CartToIso_xVector.y = -sinXY;

    m_CartToIso_yVector.x =  sinXY/sinZ;
    m_CartToIso_yVector.y =  cosXY/sinZ;

    m_TransformIsoToCart = sf::Transform(cosXY,         -sinXY,         0,
                                         sinXY * sinZ,   cosXY * sinZ, -cosZ,
                                         0,              0,             1);

     m_isoToCartMat.values[0] = cosXY;
     m_isoToCartMat.values[1] = -sinXY;
     m_isoToCartMat.values[2] = 0;
     m_isoToCartMat.values[3] = -sinXY * sinZ;
     m_isoToCartMat.values[4] = -cosXY * sinZ;
     m_isoToCartMat.values[5] = cosZ;
     m_isoToCartMat.values[6] = 0;
     m_isoToCartMat.values[7] = 0;
     m_isoToCartMat.values[8] = sinZ;

     m_cartToIsoMat.values[0] = cosXY;
     m_cartToIsoMat.values[1] = sinXY/sinZ;
     m_cartToIsoMat.values[2] = 0;
     m_cartToIsoMat.values[3] = -sinXY;
     m_cartToIsoMat.values[4] = cosXY/sinZ;
     m_cartToIsoMat.values[5] = 0;
     m_cartToIsoMat.values[6] = 0;
     m_cartToIsoMat.values[7] = 0;
     m_cartToIsoMat.values[8] = 0;


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


     m_cartToIso2DProjMat.values[0] = cosXY;
     m_cartToIso2DProjMat.values[1] = sinXY/sinZ;
     m_cartToIso2DProjMat.values[2] = 0;
     m_cartToIso2DProjMat.values[3] = -sinXY;
     m_cartToIso2DProjMat.values[4] = cosXY/sinZ;
     m_cartToIso2DProjMat.values[5] = 0;
     m_cartToIso2DProjMat.values[6] = 0;
     m_cartToIso2DProjMat.values[7] = 0;
     m_cartToIso2DProjMat.values[8] = 0;


    m_lightingShader.setUniform("cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIso2DProjMat.values));
    m_lightingShader.setUniform("isoToCartZFactor",-m_isoToCartMat.values[5]);


    m_SSAOShader.setUniform("cartToIso2DProjMat",sf::Glsl::Mat3(m_cartToIso2DProjMat.values));
    m_SSAOShader.setUniform("isoToCartZFactor",-m_isoToCartMat.values[5]);
    m_SSAOShader.setUniform("isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));

     /** ADD UPDATE OF ZFACTOR FOR CREATED ISOSPRITES**/
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
    sf::Vector2f r;
    r = m_IsoToCart_xVector * p.x + m_IsoToCart_yVector * p.y;
    return r;
}

sf::Vector2f IsometricScene::ConvertIsoToCartesian(sf::Vector3f p)
{
    sf::Vector2f r;
    r = m_IsoToCart_xVector * p.x + m_IsoToCart_yVector * p.y + m_IsoToCart_zVector * p.z;
    return r;
}


sf::Vector2f IsometricScene::ConvertCartesianToIso(float x, float y)
{
    return ConvertCartesianToIso(sf::Vector2f(x,y));
}

sf::Vector2f IsometricScene::ConvertCartesianToIso(sf::Vector2f p)
{
    sf::Vector2f r;
    r = m_CartToIso_xVector * p.x + m_CartToIso_yVector * p.y;
    return r;
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

}
