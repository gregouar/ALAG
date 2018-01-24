#include "ALAGE/gfx/IsometricScene.h"

#include "ALAGE/gfx/Sprite3DEntity.h"

namespace alag
{


const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+depthPixel.r*height + zPos);" \
    "   gl_FragColor = gl_Color * colorPixel; " \
    "}";


const std::string depthAndIllumination_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "uniform vec3 ambient_light;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   vec4 lightning = vec4(ambient_light,1.0);" \
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+depthPixel.r*height + zPos);" \
    "   gl_FragColor = gl_Color * colorPixel * lightning; " \
    "}";



const std::string illumination_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform float zPos;" \
    "uniform vec3 ambient_light;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 lightning = vec4(ambient_light,1.0);" \
    "   gl_FragColor = gl_Color * colorPixel * lightning; " \
    "}";



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
    m_depthAndIlluminationShader.loadFromMemory(depthAndIllumination_fragShader,sf::Shader::Fragment);
    m_illuminationShader.loadFromMemory(illumination_fragShader,sf::Shader::Fragment);
    SetAmbientLight(m_ambientLight);
}


IsometricScene::~IsometricScene()
{
    //dtor
}


bool IsometricScene::InitRenderer(int w, int h)
{
    m_view.setCenter(0,0);
    m_view.setSize(w, h);

    return (true);
}


void IsometricScene::ProcessRenderQueue(sf::RenderTarget *w)
{
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

        if((*renderIt)->Is3D())
        {
            if((*renderIt)->CanBeIlluminated())
                curShader = &m_depthAndIlluminationShader;
            else
                curShader = &m_depthShader;

            w->pushGLStates();
            //w->resetGLStates();

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

        } else if((*renderIt)->CanBeIlluminated())
                curShader = &m_illuminationShader;

        if(curShader != nullptr)
            curShader->setUniform("zPos",globalPos.z*DEPTH_BUFFER_NORMALISER);

        (*renderIt)->PrepareShader(curShader);
        state.shader = curShader;

        (*renderIt)->Render(w,state);

        if((*renderIt)->Is3D())
            w->popGLStates();
    }
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        glClear(GL_DEPTH_BUFFER_BIT);
        w->setView(GenerateIsoView(m_view));
        ProcessRenderQueue(w);
        w->setView(oldView);
    }
}



Sprite3DEntity* IsometricScene::CreateSprite3DEntity(sf::Vector2i spriteSize)
{
    return CreateSprite3DEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

Sprite3DEntity* IsometricScene::CreateSprite3DEntity(sf::IntRect textureRect)
{
    Sprite3DEntity *e = new Sprite3DEntity(textureRect);
    AddEntity(GenerateEntityID(), e);
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
    m_illuminationShader.setUniform("ambient_light",sf::Vector3f(
                                            (float)m_ambientLight.r/255.0,
                                            (float)m_ambientLight.g/255.0,
                                            (float)m_ambientLight.b/255.0));
    m_depthAndIlluminationShader.setUniform("ambient_light",sf::Vector3f(
                                            (float)m_ambientLight.r/255.0,
                                            (float)m_ambientLight.g/255.0,
                                            (float)m_ambientLight.b/255.0));
}

void IsometricScene::ComputeTrigonometry()
{
    float cosXY = cos(m_viewAngle.xyAngle*PI/180.0);
    float sinXY = sin(m_viewAngle.xyAngle*PI/180.0);
    float cosZ = cos(m_viewAngle.zAngle*PI/180);
    float sinZ = sin(m_viewAngle.zAngle*PI/180);

    /*m_TransformIsoToCart = sf::Transform(cosXY,         -sinXY,         0,
                                         sinXY * cosZ,   cosXY * cosZ,  sinZ,
                                         0,              0,        ,    0);

    m_TransformCartToIso = sf::Transform(cosXY, sinXY/cosZ, 0,
                                        -sinXY, cosXY/cosZ, 0,
                                         0,     0,          0);*/

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
   /* m_TransformIsoToCart = sf::Transform::Identity;
    m_TransformIsoToCart.rotate(m_viewAngle.xyAngle);
    m_TransformIsoToCart.scale(1,cosZ);*/
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

sf::Vector2f IsometricScene::ConvertCartesianToIso(sf::Vector2f p)
{
    sf::Vector2f r;
    r = m_CartToIso_xVector * p.x + m_CartToIso_yVector * p.y;
    return r;
}

sf::Vector2f IsometricScene::ConvertMouseToScene(sf::Vector2i mouse)
{
    sf::Vector2f scenePos = sf::Vector2f(mouse);
    if(m_last_target != nullptr)
    {
        sf::View oldView = m_last_target->getView();
        m_last_target->setView(GenerateIsoView(m_view));
        scenePos = sf::Vector2f(m_last_target->mapPixelToCoords(mouse));
        scenePos = ConvertCartesianToIso(scenePos);
        m_last_target->setView(oldView);
    }
    return scenePos;
}

sf::View IsometricScene::GenerateIsoView(const sf::View &view)
{
    sf::View v = view;
    v.setCenter(ConvertIsoToCartesian(view.getCenter()));
    return v;
}

}
