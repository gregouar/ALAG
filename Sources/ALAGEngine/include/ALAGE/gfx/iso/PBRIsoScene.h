#ifndef PBRISOSCENE_H
#define PBRISOSCENE_H

#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/gfx/DefaultScene.h"
#include "ALAGE/gfx/iso/IsoGeometricShadowCaster.h"
#include "ALAGE/gfx/iso/IsoWaterEntity.h"
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

struct ScreenTile
{
    sf::Vector2u position;
    std::list<SceneEntity*> entities;
    std::list<SceneEntity*> newList;
    bool askForUpdate;
};

class PBRIsoScene : public DefaultScene
{
    friend void IsoSpriteEntity::RenderShadow(sf::RenderTarget*, Light*);
    friend void IsoGeometricShadowCaster::RenderShadow(sf::RenderTarget*, Light*);
   // friend void IsoWaterEntity::Render(sf::RenderTarget*, const sf::RenderStates&);
    friend void IsoWaterEntity::RenderWaterTexture();

    public:
        PBRIsoScene();
        PBRIsoScene(IsoViewAngle);
        virtual ~PBRIsoScene();

        virtual bool InitRenderer(sf::Vector2u);
        virtual sf::View GenerateView(Camera*);
        virtual void RenderScene(sf::RenderTarget*);

        virtual void ProcessRenderQueue();
        virtual void RenderEntity(sf::RenderTarget* ,SceneEntity*);
        void RenderStaticGeometry(const sf::View &curView);
        void RenderDynamicGeometry(const sf::View &curView);
        void RenderEnvironmentMap();
        void RenderLighting();
        void RenderBloom();
        void RenderSSAO();

        IsoRectEntity*   CreateIsoRectEntity(sf::Vector2f = sf::Vector2f(0,0));
        IsoWaterEntity*  CreateIsoWaterEntity(sf::Vector2f = sf::Vector2f(0,0));
        IsoSpriteEntity* CreateIsoSpriteEntity(sf::Vector2i);
        IsoSpriteEntity* CreateIsoSpriteEntity(sf::IntRect = sf::IntRect(0,0,0,0));

        IsoGeometricShadowCaster* CreateIsoGeometricShadowCaster();

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
        void SetEnvironmentMap(TextureAsset*);
        virtual void SetShadowCasting(ShadowCastingType);
        virtual void EnableGammaCorrection();
        virtual void DisableGammaCorrection();

        void SetFoamSimulation(bool = true);
        void SetEdgeSmoothing(bool = true);
        void SetBloom(bool = true);
        void SetSSAO(bool = true);
        void SetSSR(bool = true, float = -1);

    protected:
        void ComputeTrigonometry();
        int UpdateLighting(std::multimap<float, Light*> &lightList);
        //virtual void RenderShadows(std::multimap<float, Light*> &lightList, int = GL_MAX_LIGHTS);

        void CopyPBRScreen(sf::MultipleRenderTexture *source, sf::FloatRect sourceRect,
                           sf::MultipleRenderTexture *target, sf::FloatRect targetRect, sf::Texture *depthTester = nullptr);
        void CopyPBRScreen(sf::MultipleRenderTexture *source,
                           sf::MultipleRenderTexture *target, sf::Texture *depthTester = nullptr);

        sf::Shader *GetDepthShader();
        sf::Shader *GetWaterGeometryShader();

        void CompileDepthShader();
        void CompileDepthCopierShader();
        void CompilePBRGeometryShader();
        void CompileLightingShader();
        void CompileSSAOShader();
        void CompileBlurShader();
        void CompileHDRBloomShader();
        void CompileWaterGeometryShader();

        void GenerateBrdflut();

    private:
        IsoViewAngle m_viewAngle;

        sf::Transform m_TransformIsoToCart;

        Mat3x3 m_normalProjMat;
        Mat3x3 m_normalProjMatInv;

        //Mat3x3 m_cartToIso2DProjMat;

        Mat3x3 m_isoToCartMat;
        Mat2x2 m_isoToCartMat2X2;
        Mat3x3 m_cartToIsoMat;

        //sf::Shader m_colorShader;
        //sf::Shader m_normalShader;
        sf::Shader m_depthShader;
        sf::Shader m_depthCopierShader;
        sf::Shader m_PBRGeometryShader;
        sf::Shader m_lightingShader;
        sf::Shader m_blurShader;
        sf::Shader m_HDRBloomShader;
        sf::Shader m_waterGeometryShader;
        sf::Texture m_brdf_lut;

        std::vector<ScreenTile> m_screenTiles;
        sf::Vector2u m_nbrTiles;
        sf::Vector2f m_tilesShift;
        sf::MultipleRenderTexture m_staticGeometryScreen[2];
        sf::MultipleRenderTexture m_alpha_staticGeometryScreen[2];
        bool m_swapStaticGeometryBuffers;
       // sf::MultipleRenderTexture m_staticGeometrySwapBuffer;
        bool m_firstStaticRender;
        sf::View m_lastStaticRenderView;

        int m_superSampling;

        sf::MultipleRenderTexture m_PBRScreen;
        sf::MultipleRenderTexture m_alpha_PBRScreen;
        sf::MultipleRenderTexture m_lighting_PBRScreen;
        sf::RenderTexture m_environment_PBRScreen[2];

        sf::RectangleShape m_renderer;
        sf::RenderStates m_rendererStates;

        bool m_enableBloom;
        sf::RenderTexture m_bloomScreen[2];
        sf::RectangleShape m_bloomRect;

        bool m_enableSSAO;
        sf::Shader m_SSAOShader;
        sf::RectangleShape m_SSAORect;

        //sf::Shader m_SSAODrawingShader;
        sf::RenderTexture m_SSAOScreen[2];
        sf::RectangleShape m_SSAOrenderer;
        sf::Texture m_SSAONoiseTexture;
        sf::Image m_SSAONoisePattern;

        bool m_enableSSR;

        static const IsoViewAngle DEFAULT_ISO_VIEW_ANGLE;
        static const int MAX_SHADOW_MAPS;

        static const int SCREENTILE_SIZE;

        static const std::string DEFAULT_ENABLEFOAMSIMULATION;
        static const std::string DEFAULT_ENABLEEDGESMOOTHING;
        static const std::string DEFAULT_ENABLESSAO;
        static const std::string DEFAULT_ENABLEBLOOM;
        static const std::string DEFAULT_ENABLESSR;
        static const std::string DEFAULT_SSR_THRESOLD;
        static const std::string DEFAULT_ENABLESRGB;
        static const std::string DEFAULT_SUPERSAMPLING;
        static const std::string DEFAULT_DIRECTIONALSHADOWSCASTING;
        static const std::string DEFAULT_DYNAMICSHADOWSCASTING;
        static const float BLOOM_BLUR;
        static const float BLOOM_SCREEN_RATIO;
        static const float SSAO_BLUR;
        static const float SSAO_SCREEN_RATIO;
        static const float ENVIRONMENT_BLUR;
        static const float SSAO_STRENGTH;
        static const unsigned int NBR_PARALLAX_STEPS;
};

}

#endif // ISOMETRICSCENEMANAGER_H
