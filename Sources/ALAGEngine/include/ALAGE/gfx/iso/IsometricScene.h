#ifndef ISOMETRICSCENE_H
#define ISOMETRICSCENE_H

#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/gfx/SceneManager.h"
#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "ALAGE/gfx/iso/IsoRectEntity.h"
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

        virtual bool InitRenderer(sf::Vector2u);
        virtual sf::View GenerateView(Camera*);
        virtual void ProcessRenderQueue(sf::RenderTarget*);
        virtual void RenderScene(sf::RenderTarget*);

        IsoRectEntity*   CreateIsoRectEntity(sf::Vector2f = sf::Vector2f(0,0));
        IsoSpriteEntity* CreateIsoSpriteEntity(sf::Vector2i);
        IsoSpriteEntity* CreateIsoSpriteEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        sf::Vector2f ConvertIsoToCartesian(float, float, float = 0);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector2f);
        sf::Vector2f ConvertIsoToCartesian(sf::Vector3f);
        sf::Vector2f ConvertCartesianToIso(float, float);
        sf::Vector2f ConvertCartesianToIso(sf::Vector2f);

        Mat3x3 GetIsoToCartMat();
        Mat3x3 GetCartToIsoMat();

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        void SetViewAngle(IsoViewAngle);
        void SetAmbientLight(sf::Color);

        void SetSSAO(bool);

    protected:
        void ComputeTrigonometry();
        int UpdateLighting(std::multimap<float, Light*> &lightList);

    private:
        IsoViewAngle m_viewAngle;

        sf::Vector2f m_IsoToCart_xVector;
        sf::Vector2f m_IsoToCart_yVector;
        sf::Vector2f m_IsoToCart_zVector;
        sf::Vector2f m_CartToIso_xVector;
        sf::Vector2f m_CartToIso_yVector;
        sf::Transform m_TransformIsoToCart;

        Mat3x3 m_normalProjMat;
        Mat3x3 m_normalProjMatInv;

        Mat3x3 m_cartToIso2DProjMat;

        Mat3x3 m_isoToCartMat;
        Mat3x3 m_cartToIsoMat;

        sf::Shader m_colorShader;
        sf::Shader m_normalShader;
        sf::Shader m_depthShader;
        sf::Shader m_lightingShader;

        int m_superSampling;
        sf::RenderTexture m_colorScreen;
        sf::RenderTexture m_normalScreen;
        sf::RenderTexture m_depthScreen;

        sf::RectangleShape m_renderer;
        sf::RenderStates m_rendererStates;

        bool m_enableSSAO;
        sf::Shader m_SSAOShader;
        sf::RenderTexture m_SSAOScreen;
        sf::RectangleShape m_SSAOrenderer;
        sf::Texture m_SSAONoiseTexture;
        sf::Image m_SSAONoisePattern;


        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
};

}

#endif // ISOMETRICSCENEMANAGER_H
