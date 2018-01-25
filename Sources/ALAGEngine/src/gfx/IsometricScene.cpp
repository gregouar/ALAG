#include "ALAGE/gfx/IsometricScene.h"

#include "ALAGE/gfx/Sprite3DEntity.h"
#include "ALAGE/utils/Mathematics.h"

namespace alag
{

const std::string vertexShader = \
    "varying vec3 vertex; "\
    "void main() "\
    "{ "\
    "    vertex = gl_Vertex.xyz; "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform vec3 globalPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   float zPixel = depthPixel.r*height + globalPos.z;" \
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+zPixel*0.001);" \
    "   gl_FragColor = gl_Color * colorPixel; " \
    "}";


const std::string depthAndLighting_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform sampler2D normalMap;" \
    "uniform mat3 normalProjMat;" \
    "uniform float height;" \
    "uniform vec3 globalPos;" \
    "uniform vec3 normalVec;" \
    "uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
	"	vec3 direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
	"   direction = direction * normalProjMat;"
	"   vertex.z = globalPos.z;"
    "   float zPixel = depthPixel.r*height + vertex.z;" \
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+zPixel*0.001);" \
    "   gl_FragColor = gl_Color *ambient_light * colorPixel;" \
    "int i;" \
	"for(i = 0 ; i < NBR_LIGHTS ; i = i+1)" \
	"{" \
	"	float lighting = 0.0;" \
	"	if(gl_LightSource[i].position.w == 0.0)" \
	"	{		" \
	"		vec3 light_direction = -gl_LightSource[i].position.xyz;" \
	"		lighting = max(0.0, dot(direction,normalize(light_direction)));" \
	"	}" \
	"	else" \
	"	{" \
	"		vec3 light_direction = gl_LightSource[i].position.xyz - globalPos;" \
	"		float dist = length(light_direction);" \
	"		float attenuation = 1.0/( gl_LightSource[i].constantAttenuation +" \
	"								  dist*gl_LightSource[i].linearAttenuation +" \
	"								  dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"		lighting = max(0.0, dot(direction,normalize(light_direction))) * attenuation;" \
	"	}" \
	"	lighting *= gl_LightSource[i].diffuse.a;" \
	"	gl_FragColor.rgb +=  gl_Color.rgb * colorPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;" \
	"}" \
    "}";



const std::string lighting_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D normalMap;" \
    "uniform vec3 globalPos;" \
    "uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   gl_FragColor = gl_Color * colorPixel * ambient_light; " \
    "}";



//const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE.xyAngle = 0;
//const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE.zAngle = 90;

const IsoViewAngle IsometricScene::DEFAULT_ISO_VIEW_ANGLE = {.xyAngle = 0,
                                                             .zAngle = 90};


IsometricScene::IsometricScene() : IsometricScene(DEFAULT_ISO_VIEW_ANGLE)
{
    //ctor
}

IsometricScene::IsometricScene(IsoViewAngle viewAngle)
{
    SetViewAngle(viewAngle);

    m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
    //m_depthAndLightingShader.loadFromMemory(depthAndLighting_fragShader,sf::Shader::Fragment);
    m_depthAndLightingShader.loadFromMemory(vertexShader,depthAndLighting_fragShader);
    m_lightingShader.loadFromMemory(lighting_fragShader,sf::Shader::Fragment);
    SetAmbientLight(m_ambientLight);
}


IsometricScene::~IsometricScene()
{
    //dtor
}



void IsometricScene::ProcessRenderQueue(sf::RenderTarget *w)
{
    std::multimap<float, Light*> lightList;
    m_rootNode.FindNearbyLights(&lightList);
    UpdateLighting(lightList);

    std::list<SceneEntity*>::iterator renderIt;
    for(renderIt = m_renderQueue.begin() ; renderIt != m_renderQueue.end(); ++renderIt)
    {
        sf::RenderStates state;
        state.transform = sf::Transform::Identity;

        sf::Vector3f globalPos(0,0,0);

        SceneNode *node = (*renderIt)->GetParentNode();
        if(node != nullptr)
            globalPos = node->GetGlobalPosition();

        state.transform.translate(ConvertIsoToCartesian(0,0,globalPos.z));
        state.transform *= m_TransformIsoToCart;
        state.transform.translate(globalPos.x, globalPos.y);

        sf::Shader* curShader = nullptr;

        if((*renderIt)->Is3D())
        {
            if((*renderIt)->CanBeLighted())
                curShader = &m_depthAndLightingShader;
            else
                curShader = &m_depthShader;

            w->pushGLStates();
            //w->resetGLStates();

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

        } else if((*renderIt)->CanBeLighted())
                curShader = &m_lightingShader;

        if(curShader != nullptr)
        {
            curShader->setUniform("globalPos",globalPos);
            curShader->setUniform("normalProjMat",sf::Glsl::Mat3(m_normalProjMat));
        }

        (*renderIt)->PrepareShader(curShader);
        state.shader = curShader;

        (*renderIt)->Render(w,state);

        if((*renderIt)->Is3D())
            w->popGLStates();
    }
}

void IsometricScene::RenderScene(sf::RenderTarget* w)
{
    if(w != nullptr && m_currentCamera != nullptr)
    {
        m_last_target = w;

        sf::View oldView = w->getView();
        glClear(GL_DEPTH_BUFFER_BIT);
        w->setView(GenerateView(m_currentCamera));
        ProcessRenderQueue(w);
        w->setView(oldView);
    }
}

int IsometricScene::UpdateLighting(std::multimap<float, Light*> &ligtList)
{
    int nbr_lights = SceneManager::UpdateLighting(ligtList);

    m_lightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);
    m_depthAndLightingShader.setUniform("NBR_LIGHTS",(int)nbr_lights);

    return nbr_lights;
}



Sprite3DEntity* IsometricScene::CreateSprite3DEntity(sf::Vector2i spriteSize)
{
    return CreateSprite3DEntity(sf::IntRect(0,0,spriteSize.x,spriteSize.y));
}

Sprite3DEntity* IsometricScene::CreateSprite3DEntity(sf::IntRect textureRect)
{
    Sprite3DEntity *e = new Sprite3DEntity(textureRect);
    AddCreatedObject(GenerateObjectID(), e);
    return e;
}



void IsometricScene::SetViewAngle(IsoViewAngle viewAngle)
{
    m_viewAngle = viewAngle;
    ComputeTrigonometry();
}

void IsometricScene::SetAmbientLight(sf::Color light)
{
    SceneManager::SetAmbientLight(light);

    m_lightingShader.setUniform("ambient_light",sf::Glsl::Vec4(m_ambientLight));
    m_depthAndLightingShader.setUniform("ambient_light",sf::Glsl::Vec4(m_ambientLight));
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


     m_normalProjMat[0] = cosXY;
     m_normalProjMat[1] = sinZ * sinXY;
     m_normalProjMat[2] = cosZ * sinXY;
     m_normalProjMat[3] = -sinXY;
     m_normalProjMat[4] = sinZ * cosXY;
     m_normalProjMat[5] = cosZ * cosXY;
     m_normalProjMat[6] = 0;
     m_normalProjMat[7] = -cosZ;
     m_normalProjMat[8] = sinZ;

     /*m_normalProjMat = {cosXY, sinZ*sinXY , cosZ * sinXY,
                            -sinXY, sinZ*cosXY, cosZ * cosXY,
                            0 ,  -cosZ , sinZ};*/
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


sf::Vector2f IsometricScene::ConvertCartesianToIso(float x, float y)
{
    return ConvertCartesianToIso(sf::Vector2f(x,y));
}

sf::Vector2f IsometricScene::ConvertCartesianToIso(sf::Vector2f p)
{
    sf::Vector2f r;
    r = m_CartToIso_xVector * p.x + m_CartToIso_yVector * p.y;
    return r;
}

sf::Vector2f IsometricScene::ConvertMouseToScene(sf::Vector2i mouse)
{
    sf::Vector2f scenePos = sf::Vector2f(mouse);
    if(m_last_target != nullptr && m_currentCamera != nullptr)
    {
        sf::View oldView = m_last_target->getView();
        m_last_target->setView(GenerateView(m_currentCamera));
        scenePos = sf::Vector2f(m_last_target->mapPixelToCoords(mouse));
        scenePos = ConvertCartesianToIso(scenePos);
        m_last_target->setView(oldView);
    }
    return scenePos;
}

sf::View IsometricScene::GenerateView(Camera* cam)
{
    sf::View v;
    if(cam != nullptr)
    {
        v.setSize(cam->GetSize()*cam->GetZoom());
        SceneNode *node = cam->GetParentNode();
        if(node != nullptr)
            v.setCenter(ConvertIsoToCartesian(node->GetGlobalPosition()));
    }
    return v;
}

}
