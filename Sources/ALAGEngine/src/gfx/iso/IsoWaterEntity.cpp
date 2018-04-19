#include "ALAGE/gfx/iso/PBRIsoScene.h"
#include "ALAGE/gfx/iso/IsoWaterEntity.h"
#include "ALAGE/utils/Mathematics.h"
#include "ALAGE/utils/TextureModifier.h"

namespace alag
{

const float IsoWaterEntity::DEFAULT_WAVE_STEEPNESS = 1.0;
const float IsoWaterEntity::DEFAULT_WAVE_LENGTH = 1.0;
const float IsoWaterEntity::DEFAULT_WAVE_SPEED = 0.05;
const float IsoWaterEntity::DEFAULT_WAVE_AMPLITUDE = 0.0;
const float IsoWaterEntity::DEFAULT_WAVE_TURBULENCE = 0.01;
const float IsoWaterEntity::DEFAULT_TURBULENCE_AMPLITUDE = 0.2;
const float IsoWaterEntity::DEFAULT_TURBULENCE_SPEED = 2.0;

const sf::Color IsoWaterEntity::DEFAULT_WATER_COLOR = sf::Color(30,60,60,224);
const sf::Color IsoWaterEntity::DEFAULT_WATER_MATERIAL = sf::Color(32,0,224,255);
const float IsoWaterEntity::DEFAULT_WATER_DENSITY = 1000.0f;
const sf::Color IsoWaterEntity::DEFAULT_FOAM_COLOR = sf::Color(224,224,224,224);

const sf::Vector2u IsoWaterEntity::DEFAULT_WATER_RESOLUTION = sf::Vector2u(512,512);

const size_t IsoWaterEntity::NBR_WAVE_SHAPE_SAMPLES = 192;

IsoWaterEntity::IsoWaterEntity()  : IsoWaterEntity(sf::Vector2f (0,0))
{
}

IsoWaterEntity::IsoWaterEntity(sf::Vector2f s) : IsoRectEntity(s)
{
    m_waveShape = new sf::Glsl::Vec4[NBR_WAVE_SHAPE_SAMPLES];
    m_waveXDistribution = new float[NBR_WAVE_SHAPE_SAMPLES];
    m_wavePosition = 0;
    m_waterResolution = sf::Vector2u(0,0);

    IsoRectEntity::SetParallax(true);
    RectEntity::SetTextureRect(sf::IntRect(0,0,s.x,s.y));

    SetWaterColor(DEFAULT_WATER_COLOR);
    SetWaterMaterial(DEFAULT_WATER_MATERIAL);
    SetWaterDensity(DEFAULT_WATER_DENSITY);
    SetFoamColor(DEFAULT_FOAM_COLOR);
    SetFoamActive(true);

    SetWaterResolution(DEFAULT_WATER_RESOLUTION);

    SetWaveSteepness(DEFAULT_WAVE_STEEPNESS);
    SetWaveLength(DEFAULT_WAVE_LENGTH);
    SetWaveSpeed(DEFAULT_WAVE_SPEED);
    SetWaveAmplitude(DEFAULT_WAVE_AMPLITUDE);
    SetWaveTurbulence(DEFAULT_WAVE_TURBULENCE);
    SetTurbulenceAmplitude(DEFAULT_TURBULENCE_AMPLITUDE);
    SetTurbulenceSpeed(DEFAULT_TURBULENCE_SPEED);
}

IsoWaterEntity::~IsoWaterEntity()
{
    delete m_waveShape;
    delete m_waveXDistribution;
}


void IsoWaterEntity::CreateRenderTexture()
{
    m_renderTexture.create(m_waterResolution.x,
                           m_waterResolution.y);
    m_renderTexture.addRenderTarget(PBRAlbedoScreen);
    m_renderTexture.addRenderTarget(PBRNormalScreen);
    m_renderTexture.addRenderTarget(PBRDepthScreen);
    m_renderTexture.addRenderTarget(PBRMaterialScreen);
    m_renderTexture.addRenderTarget(PBRExtraScreen0);

    for(size_t i = 0 ; i < 5 ; ++i)
    {
        m_renderTexture.setRepeated(i,true);
        m_renderTexture.setSmooth(i,true);
        if(i<4)
        m_textureAssets[i] = TextureAsset(m_renderTexture.getTexture(i));
    }

    m_PBRTextureAsset = PBRTextureAsset(&m_textureAssets[PBRAlbedoScreen],
                                     &m_textureAssets[PBRNormalScreen],
                                     &m_textureAssets[PBRDepthScreen],
                                     &m_textureAssets[PBRMaterialScreen]);

    IsoRectEntity::SetTexture(&m_PBRTextureAsset);

    for(size_t x = 0 ; x < 16 ; ++x)
    for(size_t y = 0 ; y < 16 ; ++y)
    {
        float theta = (float)RandomNumber(1000)/1000.0;
        m_noiseGradients[x+y*16].x = cos(theta*2*PI);
        m_noiseGradients[x+y*16].y = sin(theta*2*PI);
    }


    /** For seamless tiling in frequency 0.5 and 0.25**/
    for(size_t i = 0 ; i <= 4 ; ++i)
    {
        m_noiseGradients[4 + i*16] = m_noiseGradients[i*16];
        m_noiseGradients[i + 4*16] = m_noiseGradients[i];
    }

    for(size_t i = 0 ; i <= 8 ; ++i)
    {
        m_noiseGradients[8 + i*16] = m_noiseGradients[i*16];
        m_noiseGradients[i + 8*16] = m_noiseGradients[i];
    }
}


void IsoWaterEntity::PrepareShader(sf::Shader *shader)
{
    IsoRectEntity::PrepareShader(shader);

    if(shader != nullptr)
    {
        shader->setUniform("enable_volumetricOpacity",true);
        shader->setUniform("p_density",m_waterDensity);
        shader->setUniform("p_useFoam",m_enableFoam);
        shader->setUniform("p_foamColor",sf::Glsl::Vec4(m_foamColor));
        shader->setUniform("map_velocity",*m_renderTexture.getTexture(PBRExtraScreen0));
    }
}

/*void IsoWaterEntity::Render(sf::RenderTarget *w, const sf::RenderStates &state)
{
    sf::RenderStates customState = state;

    sf::Shader* waterShader = IsoRectEntity::GetIsoScene()->GetWaterGeometryShader();

    waterShader->setUniform("texture_size",sf::Vector2f(sf::RectangleShape::getTextureRect().width,
                                                        sf::RectangleShape::getTextureRect().height));

    customState.shader = waterShader;
    w->draw((*this), customState);
    AskForRenderUpdate(false);
}*/



sf::Vector3f IsoWaterEntity::TrackParticleMovement(sf::Vector2f samplePos)
{
    sf::Vector3f p(samplePos.x, samplePos.y, 0);

    sf::Vector2f textureRatio((float)m_waterResolution.x/(float)getTextureRect().width,
                              (float)m_waterResolution.y/(float)getTextureRect().height);

    samplePos.x = (int)((samplePos.x + GetCenter().x) * textureRatio.x) % m_waterResolution.x;
    samplePos.y = (int)((samplePos.y + GetCenter().y) * textureRatio.y) % m_waterResolution.y;


    //m_renderTexture.display();
    sf::Color depthPixel = m_renderTexture.getTexture(PBRDepthScreen)->copyToImage().getPixel((unsigned int)samplePos.x,
                                                                                              (unsigned int)samplePos.y);

    p.z = (float)(depthPixel.r+depthPixel.g+depthPixel.b)/(3*255.0)*m_heightFactor;


    p.x -= GetCenter().x;
    p.y -= GetCenter().y;

    return p;
}


void IsoWaterEntity::CopyNoise(IsoWaterEntity *source)
{
    if(source != nullptr)
    {
        for(size_t i = 0 ; i < 16*16 ; ++i)
            m_noiseGradients[i] = source->m_noiseGradients[i];
    }
}

float IsoWaterEntity::GetWaveSteepness()
{
    return m_waveSteepness;
}

float IsoWaterEntity::GetWaveLength()
{
    return m_waveLength;
}

float IsoWaterEntity::GetWaveSpeed()
{
    return m_waveSpeed;
}

float IsoWaterEntity::GetWaveAmplitude()
{
    return m_waveAmplitude;
}

float IsoWaterEntity::GetWaveTurbulence()
{
    return m_waveTurbulence;
}

float IsoWaterEntity::GetTurbulenceAmplitude()
{
    return m_turbulenceAmplitude;
}


float IsoWaterEntity::GetTurbulenceSpeed()
{
    return m_turbulenceSpeed;
}

sf::Color IsoWaterEntity::GetWaterColor()
{
    return m_waterColor;
}

sf::Color IsoWaterEntity::GetWaterMaterial()
{
    return m_waterMaterial;
}

float IsoWaterEntity::GetWaterDensity()
{
    return m_waterDensity;
}

sf::Color IsoWaterEntity::GetFoamColor()
{
    return m_foamColor;
}

void IsoWaterEntity::SetWaterColor(const sf::Color &c)
{
    m_waterColor = c;
}

void IsoWaterEntity::SetWaterMaterial(const sf::Color &c)
{
    m_waterMaterial = c;
}

void IsoWaterEntity::SetWaterDensity(float d)
{
    m_waterDensity = d;
}

void IsoWaterEntity::SetFoamColor(const sf::Color &c)
{
    m_foamColor = c;
}

void IsoWaterEntity::SetFoamActive(bool b)
{
    m_enableFoam = b;
}

void IsoWaterEntity::SetWaterResolution(sf::Vector2u r)
{
    if(r.x != 0 && r.y != 0)
    if(m_waterResolution != r)
    {
        m_waterResolution = r;
        CreateRenderTexture();
    }
}


void IsoWaterEntity::SetWaveSteepness(float f)
{
    //if(f > 2) f = 2;

    if(f >= 0  && m_waveSteepness != f)
    {
        m_waveSteepness = f;
       // GenerateGerstnerWave(m_waveShape, m_waveXDistribution, NBR_WAVE_SHAPE_SAMPLES,
         //                    m_waveSteepness, m_waveLength);
    }
}

void IsoWaterEntity::SetWaveLength(float f)
{
    if(f > 0 && m_waveLength != f)
    {
        m_waveLength = f;
        //GenerateGerstnerWave(m_waveShape, m_waveXDistribution, NBR_WAVE_SHAPE_SAMPLES,
          //                   m_waveSteepness, m_waveLength);
    }
}

void IsoWaterEntity::SetWaveSpeed(float f)
{
    if(f >= 0)
        m_waveSpeed = f;
    else
        m_waveSpeed = 0;
}

void IsoWaterEntity::SetWaveAmplitude(float f)
{
    if(f >= 0)
        m_waveAmplitude = f;
    else
        m_waveAmplitude = 0;
}

void IsoWaterEntity::SetWaveTurbulence(float f)
{
    m_waveTurbulence = f;
}

void IsoWaterEntity::SetTurbulenceAmplitude(float f)
{
    if(f >= 0)
        m_turbulenceAmplitude = f;
    else
        m_turbulenceAmplitude = 0;
}

void IsoWaterEntity::SetTurbulenceSpeed(float f)
{
    m_turbulenceSpeed = f;
}


void IsoWaterEntity::RenderWaterTexture()
{
    const sf::Texture &perlinNoise =
       // TextureModifier::GeneratePerlinNoise(m_noiseGradients,sf::Vector2u(16,16),
         //                                    sf::Vector2u(512,512),.5,4.0,1.0,0.5,5);
        TextureModifier::GeneratePerlinNoise(m_noiseGradients,sf::Vector2u(16,16),
                                             sf::Vector2u(512,512),.25,4.0,1.0,0.5,5);

    //perlinNoise.copyToImage().saveToFile("perlinTest.png");


    m_renderTexture.clear(sf::Color(0,0,0,0));

    sf::RenderStates state;

    sf::Shader* waterShader = IsoRectEntity::GetIsoScene()->GetWaterGeometryShader();
    waterShader->setUniform("p_height",m_heightFactor);

    float amplitudeNormalizer = 1.0;
    if(m_waveAmplitude + m_turbulenceAmplitude > 1.0)
        amplitudeNormalizer = m_waveAmplitude + m_turbulenceAmplitude;

    waterShader->setUniform("p_wave_pos",m_wavePosition);
    waterShader->setUniform("p_wave_amplitude",m_waveAmplitude/amplitudeNormalizer);
    waterShader->setUniform("p_wave_frequency",1/m_waveLength);
    waterShader->setUniform("p_wave_turbulence",m_waveTurbulence);
    waterShader->setUniform("p_turbulences_amplitude",m_turbulenceAmplitude/amplitudeNormalizer);
    waterShader->setUniformArray("p_wave_shape",m_waveShape,NBR_WAVE_SHAPE_SAMPLES);
    waterShader->setUniformArray("p_wave_xDistribution",m_waveXDistribution,NBR_WAVE_SHAPE_SAMPLES);
    waterShader->setUniform("p_wave_length",m_waveLength);

    waterShader->setUniform("p_waterColor",sf::Glsl::Vec4(m_waterColor));
    waterShader->setUniform("p_waterMaterial",sf::Glsl::Vec4(m_waterMaterial));
    waterShader->setUniform("p_foamColor",sf::Glsl::Vec4(m_foamColor));

    waterShader->setUniform("map_noise",sf::Shader::CurrentTexture);
   // waterShader->setUniform("texture_size",sf::Vector2f(m_renderTexture.getSize()));

    state.shader = waterShader;

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(m_renderTexture.getSize()));
    rect.setTexture(&perlinNoise);
    rect.setTextureRect(sf::IntRect(0,0,rect.getSize().x, rect.getSize().y));

    m_renderTexture.draw(rect, state);

    /** WE DONT NEED TO INVERSE Y-COORD**/
   // m_renderTexture.display();


    //m_renderTexture.getTexture(PBRAlbedoScreen)->copyToImage().saveToFile("water0.png");;
    //m_renderTexture.getTexture(PBRDepthScreen)->copyToImage().saveToFile("water1.png");
   /* m_renderTexture.getTexture(PBRNormalScreen)->copyToImage().saveToFile("water2.png");*/
    //m_renderTexture.getTexture(PBRMaterialScreen)->copyToImage().saveToFile("water3.png")
    //m_renderTexture.getTexture(PBRExtraScreen0)->copyToImage().saveToFile("water4.png");
}

void IsoWaterEntity::Update(const sf::Time &time)
{
    IsoRectEntity::Update(time);

  //  GenerateGerstnerWave(m_waveShape, m_waveXDistribution, NBR_WAVE_SHAPE_SAMPLES,
    //                         m_waveSteepness, m_waveLength);


 //   if(m_wavePosition == 0)

    m_wavePosition += time.asSeconds() * m_waveSpeed;

    Mat2x2 mat_rot(cos(time.asSeconds()*m_turbulenceSpeed),-sin(time.asSeconds()*m_turbulenceSpeed),
                   sin(time.asSeconds()*m_turbulenceSpeed), cos(time.asSeconds()*m_turbulenceSpeed));

    for(size_t x = 0 ; x < 16 ; ++x)
    for(size_t y = 0 ; y < 16 ; ++y)
        m_noiseGradients[x+y*16] = mat_rot * m_noiseGradients[x+y*16];


    GenerateGerstnerWave(m_waveShape, m_waveXDistribution, NBR_WAVE_SHAPE_SAMPLES, m_waveSteepness, 0, 1/m_waveLength, m_wavePosition*2*PI);

    RenderWaterTexture();
}


}
