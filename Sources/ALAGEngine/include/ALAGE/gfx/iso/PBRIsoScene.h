#ifndef PBRISOSCENE_H
#define PBRISOSCENE_H

#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/gfx/DefaultScene.h"
#include "ALAGE/gfx/iso/IsoSpriteEntity.h"
#include "ALAGE/gfx/iso/IsoRectEntity.h"
#include <SFML/OpenGL.hpp>

namespace alag
{

struct IsoViewAngle
{
    float xyAngle;
    float zAngle;
};

class PBRIsoScene : public DefaultScene
{
    friend void IsoSpriteEntity::RenderShadow(sf::RenderTarget*, Light*);

    public:
        PBRIsoScene();
        PBRIsoScene(IsoViewAngle);
        virtual ~PBRIsoScene();

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
        const sf::Transform& GetIsoToCartTransform();

        virtual sf::Vector2f ConvertMouseToScene(sf::Vector2i);

        void SetViewAngle(IsoViewAngle);
        void SetAmbientLight(sf::Color);
        virtual void SetShadowCasting(ShadowCastingType);
        virtual void EnableGammaCorrection();
        virtual void DisableGammaCorrection();

        void SetSSAO(bool);

    protected:
        void ComputeTrigonometry();
        int UpdateLighting(std::multimap<float, Light*> &lightList);
        //virtual void RenderShadows(std::multimap<float, Light*> &lightList, int = GL_MAX_LIGHTS);

        void CompileDepthShader();
        void CompilePBRGeometryShader();
        void CompileLightingShader();
        void CompileSSAOShader();

        sf::Shader *GetDepthShader();

    private:
        IsoViewAngle m_viewAngle;

        sf::Transform m_TransformIsoToCart;

        Mat3x3 m_normalProjMat;
        Mat3x3 m_normalProjMatInv;

        //Mat3x3 m_cartToIso2DProjMat;

        Mat3x3 m_isoToCartMat;
        Mat3x3 m_cartToIsoMat;

        //sf::Shader m_colorShader;
        //sf::Shader m_normalShader;
        sf::Shader m_depthShader;
        sf::Shader m_PBRGeometryShader;
        sf::Shader m_lightingShader;

        /*sf::RenderTexture m_colorScreen;
        sf::RenderTexture m_normalScreen;
        sf::RenderTexture m_depthScreen;*/

        int m_superSampling;
        sf::MultipleRenderTexture m_PBRScreen;
        sf::MultipleRenderTexture m_alpha_PBRScreen;

        sf::RectangleShape m_renderer;
        sf::RenderStates m_rendererStates;

        bool m_enableSSAO;
        sf::Shader m_SSAOShader;
        sf::RenderTexture m_SSAOScreen;
        sf::RectangleShape m_SSAOrenderer;
        sf::Texture m_SSAONoiseTexture;
        sf::Image m_SSAONoisePattern;


        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
        static const int MAX_SHADOW_MAPS;

        static const std::string DEFAULT_ENABLESSAO;
        static const std::string DEFAULT_ENABLESRGB;
        static const std::string DEFAULT_SUPERSAMPLING;
        static const std::string DEFAULT_DIRECTIONALSHADOWSCASTING;
        static const std::string DEFAULT_DYNAMICSHADOWSCASTING;
};

}

#endif // ISOMETRICSCENEMANAGER_H
