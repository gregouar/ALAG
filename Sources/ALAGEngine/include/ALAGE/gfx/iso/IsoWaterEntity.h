#ifndef ISOWATER_H
#define ISOWATER_H

#include <ALAGE/gfx/iso/IsoRectEntity.h>

namespace alag
{

class IsoWaterEntity : public IsoRectEntity
{
    public:
        IsoWaterEntity();
        IsoWaterEntity(sf::Vector2f rectSize);
        virtual ~IsoWaterEntity();

        void RenderWaterTexture();

        virtual void Update(const sf::Time &);

        virtual void PrepareShader(sf::Shader *);

        sf::Vector3f TrackParticleMovement(sf::Vector2f);

        void CopyNoise(IsoWaterEntity *source);

        float GetWaveSteepness();
        float GetWaveLength();
        float GetWaveSpeed();
        float GetWaveAmplitude();
        float GetWaveTurbulence();
        float GetTurbulenceAmplitude();
        float GetTurbulenceSpeed();
        sf::Color GetWaterColor();
        sf::Color GetWaterMaterial();
        float GetWaterDensity();
        sf::Color GetFoamColor();

        void SetWaterResolution(sf::Vector2u);

        void SetWaveSteepness(float);
        void SetWaveLength(float);
        void SetWaveSpeed(float); /* Not negative, use rotation for negative direction */
        void SetWaveAmplitude(float); /* Not negative, normalize with turbulence amplitude if sum more than 1 */
        void SetWaveTurbulence(float); /* Add Perlin noise to wave shape */
        void SetTurbulenceAmplitude(float);  /* Not negative, normalize with wave amplitude if sum more than 1 */
        void SetTurbulenceSpeed(float);
        void SetWaterColor(const sf::Color &);
        void SetWaterMaterial(const sf::Color &);
        void SetWaterDensity(float);
        void SetFoamColor(const sf::Color &);
        void SetFoamActive(bool = true);


        static const size_t NBR_WAVE_SHAPE_SAMPLES;

    protected:
        void CreateRenderTexture();

    private:

        sf::MultipleRenderTexture m_renderTexture;
        TextureAsset m_textureAssets[4];
        PBRTextureAsset m_PBRTextureAsset;

        sf::Vector2u m_waterResolution;
        float m_waveSpeed;
        float m_waveSteepness;
        float m_waveLength;
        float m_waveAmplitude;
        float m_waveTurbulence;
        float m_turbulenceAmplitude;
        float m_turbulenceSpeed;
        sf::Color m_waterColor;
        sf::Color m_waterMaterial;
        bool m_enableFoam;
        float m_waterDensity;
        sf::Color m_foamColor;

        TextureAsset* m_waterTexture;
        bool m_isWaterTexturePBR;

        sf::Vector2f m_noiseGradients[16*16];
        sf::Glsl::Vec4 *m_waveShape;
        float *m_waveXDistribution;
        float m_wavePosition;

        static const sf::Vector2u DEFAULT_WATER_RESOLUTION;

        static const float DEFAULT_WAVE_STEEPNESS;
        static const float DEFAULT_WAVE_LENGTH;
        static const float DEFAULT_WAVE_SPEED;
        static const float DEFAULT_WAVE_AMPLITUDE;
        static const float DEFAULT_WAVE_TURBULENCE;
        static const float DEFAULT_TURBULENCE_AMPLITUDE;
        static const float DEFAULT_TURBULENCE_SPEED;

        static const sf::Color DEFAULT_WATER_COLOR;
        static const sf::Color DEFAULT_WATER_MATERIAL;
        static const float DEFAULT_WATER_DENSITY;
        static const sf::Color DEFAULT_FOAM_COLOR;
};

}

#endif // ISOWATER_H

