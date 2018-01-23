#include "ALAGE/gfx/IsometricScene.h"

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

IsometricScene::IsometricScene(IsoViewAngle viewAngle) : SceneManager()
{
    SetViewAngle(viewAngle);
}


IsometricScene::~IsometricScene()
{
    //dtor
}


bool IsometricScene::InitRenderer(int w, int h)
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    m_view.setCenter(0,0);
    m_view.setSize(w, h);

    return (true);
}


void IsometricScene::ProcessRenderQueue(sf::RenderTarget *w)
{
    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::Transform totalTransform;
        totalTransform = sf::Transform::Identity;

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            totalTransform.translate(ConvertIsoToCartesian(0,0,node->GetGlobalPosition().z));

        totalTransform *= m_TransformIsoToCart;

        (*renderIt)->Render(w,totalTransform);
    }
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr)
    {
        sf::View oldView = w->getView();
        glClearDepth(1.f);
        w->setView(GenerateIsoView(m_view));

        ProcessRenderQueue(w);

        w->setView(oldView);
    }
}

void IsometricScene::SetViewAngle(IsoViewAngle viewAngle)
{
    m_viewAngle = viewAngle;
    ComputeTrigonometry();
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

sf::View IsometricScene::GenerateIsoView(const sf::View &view)
{
    sf::View v = view;
    v.setCenter(ConvertIsoToCartesian(view.getCenter()));
    return v;
}

}
