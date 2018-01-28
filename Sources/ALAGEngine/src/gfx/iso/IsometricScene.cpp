#include "ALAGE/gfx/iso/IsometricScene.h"

#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "ALAGE/utils/Mathematics.h"
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

    m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
    //m_depthAndLightingShader.loadFromMemory(depthAndLighting_fragShader,sf::Shader::Fragment);
    m_depthAndLightingShader.loadFromMemory(vertexShader,depthAndLighting_fragShader);
    m_lightingShader.loadFromMemory(lighting_fragShader,sf::Shader::Fragment);
    SetAmbientLight(m_ambientLight);
}


IsometricScene::~IsometricScene()
{
    //dtor
}



void IsometricScene::ProcessRenderQueue(sf::RenderTarget *w)
{
    std::multimap<float, Light*> lightList;
    m_rootNode.FindNearbyLights(&lightList);
    UpdateLighting(lightList);

    w->pushGLStates();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

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

        /*if((*renderIt)->Is3D())
        {
            if((*renderIt)->CanBeLighted())
                curShader = &m_depthAndLightingShader;
            else
                curShader = &m_depthShader;

            w->pushGLStates();
            //w->resetGLStates();

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

        } else if((*renderIt)->CanBeLighted())
                curShader = &m_lightingShader;*/

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
        }

        (*renderIt)->PrepareShader(curShader);
        state.shader = curShader;

        (*renderIt)->Render(w,state);

        /*if((*renderIt)->Is3D())
            w->popGLStates();*/
    }
    w->popGLStates();
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr && m_currentCamera != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        glClear(GL_DEPTH_BUFFER_BIT);
        w->setView(GenerateView(m_currentCamera));
        ProcessRenderQueue(w);
        w->setView(oldView);
    }
}

int IsometricScene::UpdateLighting(std::multimap<float, Light*> &ligtList)
{
    int nbr_lights = SceneManager::UpdateLighting(ligtList);

    m_lightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);
    m_depthAndLightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);

    return nbr_lights;
}



IsoSpriteEntity* IsometricScene::CreateIsoSpriteEntity(sf::Vector2i spriteSize)
{
    return CreateIsoSpriteEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

IsoSpriteEntity* IsometricScene::CreateIsoSpriteEntity(sf::IntRect textureRect)
{
    IsoSpriteEntity *e = new IsoSpriteEntity(textureRect);
    AddCreatedObject(GenerateObjectID(), e);
    e->SetIsoToCartZFactor(m_isoToCartZFactor);
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
    m_depthAndLightingShader.setUniform("ambient_light",sf::Glsl::Vec4(m_ambientLight));
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


     m_normalProjMat[0] = cosXY;
     m_normalProjMat[1] = sinZ * sinXY;
     m_normalProjMat[2] = cosZ * sinXY;
     m_normalProjMat[3] = -sinXY;
     m_normalProjMat[4] = sinZ * cosXY;
     m_normalProjMat[5] = cosZ * cosXY;
     m_normalProjMat[6] = 0;
     m_normalProjMat[7] = -cosZ;
     m_normalProjMat[8] = sinZ;

     m_cartToIso2DProjMat[0] = cosXY;
     m_cartToIso2DProjMat[1] = sinXY/sinZ;
     m_cartToIso2DProjMat[2] = 0;
     m_cartToIso2DProjMat[3] = -sinXY;
     m_cartToIso2DProjMat[4] = cosXY/sinZ;
     m_cartToIso2DProjMat[5] = 0;
     m_cartToIso2DProjMat[6] = 0;
     m_cartToIso2DProjMat[7] = 0;
     m_cartToIso2DProjMat[8] = 0;

     m_isoToCartZFactor = -cosZ;

     /** ADD UPDATE OF ZFACTOR FOR CREATED ISOSPRITES**/
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
