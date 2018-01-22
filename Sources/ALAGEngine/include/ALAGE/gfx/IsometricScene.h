#ifndef ISOMETRICSCENE_H
#define ISOMETRICSCENE_H

#include "ALAGE/gfx/SceneManager.h"
#include <SFML/OpenGL.hpp>

namespace alag
{

struct IsoViewAngle
{
    float xyAngle;
    float zAngle;
};

class IsometricScene : public SceneManager
{
    public:
        IsometricScene();
        IsometricScene(IsoViewAngle);
        virtual ~IsometricScene();

        virtual bool InitRenderer(int, int);
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*);

        sf::Vector2f ConvertIsoToCartesian(float, float, float = 0);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector2f);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector3f);
        sf::Vector2f ConvertCartesianToIso(sf::Vector2f);

        void SetViewAngle(IsoViewAngle);
        void ComputeTrigonometry();

        sf::View GenerateIsoView(const sf::View &);

    protected:

    private:
        IsoViewAngle m_viewAngle;
        sf::Vector2f m_IsoToCart_xVector;
        sf::Vector2f m_IsoToCart_yVector;
        sf::Vector2f m_IsoToCart_zVector;
        sf::Vector2f m_CartToIso_xVector;
        sf::Vector2f m_CartToIso_yVector;
        sf::Transform m_TransformIsoToCart;

        //sf::Vector3f m_camera; USE m_view INSTEAD

        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
};

}

#endif // ISOMETRICSCENEMANAGER_H
