#ifndef ISOMETRICSCENE_H
#define ISOMETRICSCENE_H

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

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        void SetViewAngle(IsoViewAngle);
        void SetAmbientLight(sf::Color);

        void ComputeTrigonometry();

    protected:
        int UpdateLighting(std::multimap<float, Light*> &lightList);

    private:
        IsoViewAngle m_viewAngle;

        sf::Vector2f m_IsoToCart_xVector;
        sf::Vector2f m_IsoToCart_yVector;
        sf::Vector2f m_IsoToCart_zVector;
        sf::Vector2f m_CartToIso_xVector;
        sf::Vector2f m_CartToIso_yVector;
        sf::Transform m_TransformIsoToCart;
        GLfloat m_normalProjMat[9];
        GLfloat m_normalProjMatInv[9];
        GLfloat m_cartToIso2DProjMat[9];
        GLfloat m_isoToCartMat[9];
        GLfloat m_isoToCartZFactor;

        sf::Shader m_depthShader;
        sf::Shader m_depthAndLightingShader;
        sf::Shader m_lightingShader;

        bool m_enableSSAO;
        bool m_useSecondScreen;
        sf::RenderTexture m_geometryScreen[2];
        sf::Shader m_geometryShader;

        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
};

}

#endif // ISOMETRICSCENEMANAGER_H
