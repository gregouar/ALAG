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


bool IsometricScene::InitRenderer()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    return (true);
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr)
    {
        sf::View oldView = w->getView();
        glClearDepth(1.f);
        w->setView(GenerateIsoView(m_view));

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

    m_IsoToCart_xVector.x =  cosXY;
    m_IsoToCart_xVector.y =  sinXY * cosZ;

    m_IsoToCart_yVector.x = -sinXY;
    m_IsoToCart_yVector.y =  cosXY * cosZ;

    m_IsoToCart_zVector.x =  0;
    m_IsoToCart_zVector.y =  sinZ;

    m_CartToIso_xVector.x =  cosXY;
    m_CartToIso_xVector.y = -sinXY;

    m_CartToIso_yVector.x =  sinXY/cosZ;
    m_CartToIso_yVector.y =  cosXY/cosZ;
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
