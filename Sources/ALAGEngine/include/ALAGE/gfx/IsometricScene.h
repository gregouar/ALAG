#ifndef ISOMETRICSCENE_H
#define ISOMETRICSCENE_H

#include "ALAGE/gfx/SceneManager.h"
#include "ALAGE/gfx/Sprite3DEntity.h"
#include <SFML/OpenGL.hpp>

namespace alag
{

const float DEPTH_BUFFER_NORMALISER = 0.001f;

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

        virtual sf::View GenerateView(Camera*);
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*);

        Sprite3DEntity* CreateSprite3DEntity(sf::Vector2i);
        Sprite3DEntity* CreateSprite3DEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        sf::Vector2f ConvertIsoToCartesian(float, float, float = 0);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector2f);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector3f);
        sf::Vector2f ConvertCartesianToIso(float, float);
        sf::Vector2f ConvertCartesianToIso(sf::Vector2f);

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        void SetViewAngle(IsoViewAngle);
        void SetAmbientLight(sf::Color);

        void ComputeTrigonometry();

    protected:
        size_t UpdateLighting(std::multimap<float, Light*> &lightList);

    private:
        IsoViewAngle m_viewAngle;
        sf::Vector2f m_IsoToCart_xVector;
        sf::Vector2f m_IsoToCart_yVector;
        sf::Vector2f m_IsoToCart_zVector;
        sf::Vector2f m_CartToIso_xVector;
        sf::Vector2f m_CartToIso_yVector;
        sf::Transform m_TransformIsoToCart;

        sf::Shader m_depthShader;
        sf::Shader m_depthAndLightingShader;
        sf::Shader m_lightingShader;

        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
};

}

#endif // ISOMETRICSCENEMANAGER_H
