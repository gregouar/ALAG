#include "ALAGE/gfx/iso/PBRIsoScene.h"

#include <sstream>
#include <iostream>

namespace alag
{

void PBRIsoScene::CompileDepthShader()
{
    std::ostringstream fragShader;

    fragShader<<
    "uniform sampler2D map_albedo;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(map_albedo, gl_TexCoord[0].xy).a;" \
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "   }"
    "   float zPixel = heightPixel + p_zPos;" \
    "   gl_FragDepth = 1.0 - colorAlpha*(0.5+zPixel*"
    << DEPTH_BUFFER_NORMALISER <<
    ");" \
    "   gl_FragColor.r = floor(gl_FragDepth * 255.0)/255.0;" \
    "   gl_FragColor.g = floor((gl_FragDepth -  floor(gl_FragDepth * 255.0)/255.0)*65025.0)/255.0;" \
    "   gl_FragColor.b = (gl_FragDepth - floor(gl_FragDepth * 65025.0)/65025.0)*65025.0;" \
    "   gl_FragColor.a = colorAlpha;" \
    "}";

    m_depthShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
}

void PBRIsoScene::CompilePBRGeometryShader()
{
    std::ostringstream fragShader;

    fragShader<<
    "#version 330 compatibility \n"
    "layout (location = 0) out vec4 AlbedoColor;"
    "layout (location = 1) out vec4 NormalColor;"
    "layout (location = 2) out vec4 DepthColor;"
    "layout (location = 3) out vec4 MaterialColor;"
    "uniform bool p_alpha_pass;"
    "uniform sampler2D map_albedo;"  \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform bool enable_normalMap;" \
    "uniform sampler2D map_normal;" \
    "uniform bool enable_materialMap;" \
    "uniform sampler2D map_material;" \
    "uniform float p_roughness;" \
    "uniform float p_metalness;" \
    "uniform float p_translucency;" \
    "uniform mat3 p_normalProjMat;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   gl_FragDepth = 1.0;"
    "   vec4 albedoPixel = texture2D(map_albedo, gl_TexCoord[0].xy);" \
    "   if((albedoPixel.a > .2 && albedoPixel.a < .9 && p_alpha_pass == true) "
    "   || (albedoPixel.a >= .9 && p_alpha_pass == false))"
    "   {"
	"	    vec3 direction = vec3(0,0,-1);"
	"       if(enable_normalMap == true){"
	"        direction = -1.0+2.0*texture2D(map_normal, gl_TexCoord[0].xy).rgb;"
	"       }"
	"       direction = direction * p_normalProjMat;"
    "       float heightPixel = 0; "
    "       if(enable_depthMap == true){"
    "           vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "           heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "       }"
    "       float zPixel = heightPixel + p_zPos;" \
    "       gl_FragDepth = 1.0 - (0.5+zPixel*"
    << DEPTH_BUFFER_NORMALISER << ");"
    "       vec4 materialPixel = vec4(p_roughness,p_metalness,p_translucency,1.0);"
    "       if(enable_materialMap == true){"
    "           materialPixel.rgb = texture2D(map_material, gl_TexCoord[0].xy).rgb;"
    "       }"
    "       AlbedoColor = gl_Color*albedoPixel; " \
    "       NormalColor.rgb = 0.5+direction*0.5;" \
    "       NormalColor.a = 1.0;" \
    "       DepthColor.r = floor(gl_FragDepth * 255.0)/255.0;" \
    "       DepthColor.g = floor((gl_FragDepth -  floor(gl_FragDepth * 255.0)/255.0)*65025.0)/255.0;" \
    "       DepthColor.b = (gl_FragDepth - floor(gl_FragDepth * 65025.0)/65025.0)*65025.0;" \
    "       DepthColor.a = 1.0;" \
    "       MaterialColor = materialPixel; " \
    "   }"
    "}";

    m_PBRGeometryShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
}


/** Lighting inspired by tutorials on https://learnopengl.com **/

void PBRIsoScene::CompileLightingShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec3 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform sampler2D map_albedo;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_material;" \
    "uniform sampler2D alpha_albedo;" \
    "uniform sampler2D alpha_normal;" \
    "uniform sampler2D alpha_depth;" \
    "uniform sampler2D alpha_material;" \
    "uniform float p_zPos;" \
    "uniform float view_zoom;"
    "uniform vec2 view_ratio;" \
    "uniform vec2 view_shift;" \
    "uniform vec3 view_pos;" \
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform float p_isoToCartZFactor;" \
    "uniform float p_exposure;"
    "uniform vec4 light_ambient;" \
    "uniform int light_nbr;" \
    "uniform int shadow_caster["<<MAX_SHADOW_MAPS<<"];";
    for(int i = 0 ; i < MAX_SHADOW_MAPS ; ++i)
        fragShader<<"uniform sampler2D shadow_map_"<<i<<";";
    fragShader<<
    "uniform vec2 shadow_shift["<<MAX_SHADOW_MAPS<<"];"
    "uniform vec2 shadow_ratio["<<MAX_SHADOW_MAPS<<"];"
    "uniform bool enable_directionalShadows;" \
    "uniform bool enable_dynamicShadows;" \
    "uniform bool enable_SSAO;" \
    "uniform sampler2D map_SSAO;" \
    "uniform bool enable_sRGB;" \
    "varying vec3 v_vertex; "\
    ""
    " const float PI = 3.14159265359;"
    ""
    "float GetShadowCastValue(int curShadowMap, float heightPixel, vec2 shadowPos) {"
    "   vec4 shadowPixel;"
    "   vec2 mapPos = (shadowPos-shadow_shift[curShadowMap])*shadow_ratio[curShadowMap];";
    for(int i = 0 ; i < MAX_SHADOW_MAPS ; ++i)
        fragShader<<"if(curShadowMap == "<<i<<")"
        <<"shadowPixel = texture2D(shadow_map_"<<i<<", mapPos);";
    fragShader<<
	"   float shadowHeight = (0.5-(shadowPixel.r+shadowPixel.g/255.0+shadowPixel.b/65025.0))*"
	<< DEPTH_BUFFER_NORMALISER_INV <<
	";"
    "   return 1.0 - min(1.0,max(0.0, (shadowHeight-heightPixel)*0.05));"
    "}"
    ""
    "vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)"
    "{"
    "    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);"
    "}   "
    ""
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)"
    "{"
    "    return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);"
    "}"
    ""
    "float DistributionGGX(vec3 N, vec3 H, float roughness)"
    "{"
    "    float a      = roughness*roughness;"
    "    float a2     = a*a;"
    "    float NdotH  = max(dot(N, H), 0.0);"
    "    float NdotH2 = NdotH*NdotH;"
    "    float nom   = a2;"
    "    float denom = (NdotH2 * (a2 - 1.0) + 1.0);"
    "    denom = PI * denom * denom;"
    "    return nom / denom;"
    "}"
    "float GeometrySchlickGGX(float NdotV, float roughness)"
    "{"
    "    float r = (roughness + 1.0);"
    "    float k = (r*r) / 8.0;"
    "    float nom   = NdotV;"
    "    float denom = NdotV * (1.0 - k) + k;"
    "    return nom / denom;"
    "}"
    "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)"
    "{"
    "    float NdotV = max(dot(N, V), 0.0);"
    "    float NdotL = max(dot(N, L), 0.0);"
    "    float ggx2  = GeometrySchlickGGX(NdotV, roughness);"
    "    float ggx1  = GeometrySchlickGGX(NdotL, roughness);"
    "    return ggx1 * ggx2;"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 albedoPixel  = texture2D(map_albedo, gl_TexCoord[0].xy);" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel  = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec4 materialPixel  = texture2D(map_material, gl_TexCoord[0].xy);" \
    "   vec3 direction = normalize(-1.0+2.0*normalPixel.rgb);"
    "   if(enable_sRGB == true)"
	"       albedoPixel.rgb = pow(albedoPixel.rgb, vec3(2.2));"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/255.0+depthPixel.b/65025.0))*"
    << DEPTH_BUFFER_NORMALISER_INV <<
    ";"
    "   bool enable_alpha = false;"
    "   vec4 a_albedoPixel = texture2D(alpha_albedo, gl_TexCoord[0].xy);"
    "   float a_heightPixel = 0;"
    "   if(a_albedoPixel.a > .1){"
    "       vec4 a_depthPixel = texture2D(alpha_depth, gl_TexCoord[0].xy);"
    "       a_heightPixel = (0.5-(a_depthPixel.r+a_depthPixel.g/255.0+a_depthPixel.b/65025.0))*"
    << DEPTH_BUFFER_NORMALISER_INV <<";"
    "       if(a_heightPixel > heightPixel)"
    "           enable_alpha = true;"
    "   }"
    "   vec3 a_direction;"
    "   vec4 a_materialPixel;"
    "   if(enable_alpha == true) {"
    "       vec4 a_normalPixel = texture2D(alpha_normal, gl_TexCoord[0].xy);" \
    "       vec4 a_materialPixel  = texture2D(alpha_material, gl_TexCoord[0].xy);" \
    "       a_direction = normalize(-1.0+2.0*a_normalPixel.rgb);"
    "       if(enable_sRGB == true)"
	"           a_albedoPixel.rgb = pow(a_albedoPixel.rgb, vec3(2.2));"
    "   }"
    ""
	"   vec3 fragPos = v_vertex*view_zoom+vec3(view_shift.xy,0);"
	"   vec3 a_fragPos = fragPos;"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
	"   if(enable_alpha == true){"
	"       a_fragPos.y -= a_heightPixel*p_isoToCartZFactor;"
	"       a_fragPos = a_fragPos*p_cartToIso2DProjMat;"
	"       a_fragPos.z = a_heightPixel;"
	"   }"
	""
    "   vec3 viewDirection = normalize(view_pos - fragPos);"
    "   vec3 surfaceReflection0 = vec3(0.04);"
    "   surfaceReflection0 = mix(surfaceReflection0, albedoPixel.rgb, materialPixel.g);"
    "   float FAmbient = fresnelSchlickRoughness(max(dot(direction, viewDirection), 0.0), surfaceReflection0, materialPixel.r);"
    "   vec3 kSAmbient = FAmbient;"
    "   vec3 kDAmbient = (1.0 - kSAmbient)*(1.0 - materialPixel.g);"
    "   vec3 irradianceAmbient = light_ambient.rgb*light_ambient.a;"
    "   gl_FragColor.rgb = (albedoPixel.rgb  * kDAmbient + kSAmbient)* irradianceAmbient; "
    "   gl_FragColor.a = albedoPixel.a; "
    ""
    "   vec4 alpha_FragColor;"
    "   vec3 a_surfaceReflection0 = vec3(0.04);"
    "   vec3 a_viewDirection;"
    "   if(enable_alpha == true) {"
    "       a_viewDirection = normalize(view_pos - a_fragPos);"
    "       a_surfaceReflection0 = mix(a_surfaceReflection0, a_albedoPixel.rgb, a_materialPixel.g);"
    "       float a_FAmbient = fresnelSchlickRoughness(max(dot(a_direction, a_viewDirection), 0.0), a_surfaceReflection0, a_materialPixel.r);"
    "       vec3 a_kSAmbient = a_FAmbient;"
    "       vec3 a_kDAmbient = (1.0 - a_kSAmbient)*(1.0 - a_materialPixel.g);"
    "       alpha_FragColor.rgb = (a_albedoPixel.rgb  * a_kDAmbient + a_kSAmbient)* irradianceAmbient; "
    "       alpha_FragColor.a = a_albedoPixel.a; "
    "   }"
    ""
    "   int curShadowMap = 0;"
    "   for(int i = 0 ; i < light_nbr ; ++i)" \
	"   {" \
	"	    float attenuation = 0.0;" \
	"       vec3 lightDirection = vec3(0,0,0);"
	"	    float a_attenuation = 0.0;" \
	"       vec3 a_lightDirection = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	lightDirection = -gl_LightSource[i].position.xyz;" \
	"	    	a_lightDirection = -gl_LightSource[i].position.xyz;" \
    "           attenuation = 1.0;"
    "           a_attenuation = 1.0;"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	lightDirection = gl_LightSource[i].position.xyz - fragPos.xyz;" \
	"	    	float dist = length(lightDirection)*0.01;" \
	"           float dr = dist/gl_LightSource[i].constantAttenuation;"
	"           float sqrtnom = 1.0 - dr*dr*dr*dr;"
    "           if(sqrtnom >= 0)"
	"               attenuation = saturate(sqrtnom*sqrtnom/(dist*dist+1.0));"
	""
	"           if(enable_alpha){"
	"	    	    a_lightDirection = gl_LightSource[i].position.xyz - a_fragPos.xyz;" \
	"	    	    float a_dist = length(a_lightDirection)*0.01;" \
	"               float a_dr = a_dist/gl_LightSource[i].constantAttenuation;"
	"               float a_sqrtnom = 1.0 - a_dr*a_dr*a_dr*a_dr;"
    "               if(a_sqrtnom >= 0)"
	"                   a_attenuation = saturate(a_sqrtnom*a_sqrtnom/(a_dist*a_dist+1.0));"
	"           }"
	""
	"	    }" \
    "       if((gl_LightSource[i].position.w == 0.0 && enable_directionalShadows == true)"
    "        ||(gl_LightSource[i].position.w != 0.0 && enable_dynamicShadows == true))"
	"       if(curShadowMap < "<<MAX_SHADOW_MAPS<<" && shadow_caster[curShadowMap] == i) {"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec2 shadowPos = gl_FragCoord.xy*view_zoom;"
	"               shadowPos.y += heightPixel*p_isoToCartZFactor;"
	"               vec3 v = vec3((heightPixel*lightDirection.xy/lightDirection.z), 0);"
	"               shadowPos.x -= (v*p_isoToCartMat).x;"
	"               shadowPos.y += (v*p_isoToCartMat).y;"
	//              SHOULD DO 4 TESTS FIRST AND THEN REFINE
	"               attenuation *= (GetShadowCastValue(curShadowMap,heightPixel,shadowPos)*4"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,2))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,-2))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(1,1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-1,1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-1,-1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(1,-1)))/16;"
    ""
    "               if(enable_alpha){"
    "                   vec2 a_shadowPos = gl_FragCoord.xy*view_zoom;"
	"                   a_shadowPos.y += a_heightPixel*p_isoToCartZFactor;"
	"                   vec3 a_v = vec3((a_heightPixel*a_lightDirection.xy/a_lightDirection.z), 0);"
	"                   a_shadowPos.x -= (a_v*p_isoToCartMat).x;"
	"                   a_shadowPos.y += (a_v*p_isoToCartMat).y;"
	"                   a_attenuation *= (GetShadowCastValue(curShadowMap,a_heightPixel,a_shadowPos));"
   /* "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,2))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,-2))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(1,1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-1,1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-1,-1))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(1,-1)))/16;"*/
    "               }"
    ""
	"           }"
	"           ++curShadowMap;"
	"       }"
	"       lightDirection = normalize(lightDirection);"
    "       vec3 halfwayVector = normalize(viewDirection + lightDirection);"
    "       vec3 radiance = gl_LightSource[i].diffuse * attenuation; "
    "       float NDF = DistributionGGX(direction, halfwayVector, materialPixel.r); "
    "       float G   = GeometrySmith(direction, viewDirection, lightDirection, materialPixel.r);"
    "       vec3 F    = fresnelSchlick(max(dot(halfwayVector, viewDirection), 0.0), surfaceReflection0); "
    "       vec3 kS = F;"
    "       vec3 kD = vec3(1.0) - kS;"
    "       kD *= 1.0 - materialPixel.g;"
    "       vec3 nominator    = NDF * G * F;"
    "       float denominator = 4.0 * max(dot(direction, viewDirection), 0.0) * max(dot(direction, lightDirection), 0.0);"
    "       vec3 specular     = nominator / max(denominator, 0.01);"
    "       float NdotL = max(dot(direction, lightDirection), 0.0);"
	"	    gl_FragColor.rgb += (kD * albedoPixel.rgb / PI + specular) * radiance * NdotL;"
	"       float t = materialPixel.b;"
	"	    gl_FragColor.rgb -= (albedoPixel.rgb/ PI) * radiance * min(dot(direction, lightDirection), 0.0)*t;"
	""
	"       if(enable_alpha){"
	"           a_lightDirection = normalize(a_lightDirection);"
    "           vec3 a_halfwayVector = normalize(a_viewDirection + a_lightDirection);"
    "           vec3 a_radiance = gl_LightSource[i].diffuse * a_attenuation; "
    "           float a_NDF = DistributionGGX(a_direction, a_halfwayVector, a_materialPixel.r); "
    "           float a_G   = GeometrySmith(a_direction, a_viewDirection, a_lightDirection, a_materialPixel.r);"
    "           vec3 a_F    = fresnelSchlick(max(dot(a_halfwayVector, a_viewDirection), 0.0), a_surfaceReflection0); "
    "           vec3 a_kS = a_F;"
    "           vec3 a_kD = vec3(1.0) - a_kS;"
    "           a_kD *= 1.0 - a_materialPixel.g;"
    "           vec3 a_nominator    = a_NDF * a_G * a_F;"
    "           float a_denominator = 4.0 * max(dot(a_direction, a_viewDirection), 0.0) * max(dot(a_direction, a_lightDirection), 0.0);"
    "           vec3 a_specular     = a_nominator / max(a_denominator, 0.01);"
    "           float a_NdotL = max(dot(a_direction, a_lightDirection), 0.0);"
	"	        alpha_FragColor.rgb += (a_kD * a_albedoPixel.rgb / PI + a_specular) * a_radiance * a_NdotL;"
	//"           float a_t = max(a_materialPixel.b, a_albedoPixel.a);"
	"           float a_t = a_materialPixel.b;"
	"	        alpha_FragColor.rgb -= (a_albedoPixel.rgb/ PI) * a_radiance * min(dot(a_direction, a_lightDirection), 0.0)*a_t;"
	"       }"
	""
	"   }"
	//"   gl_FragColor.rgb = gl_FragColor.rgb/(gl_FragColor.rgb+vec3(1.0));"
	"   if(enable_alpha == true){"
    "       gl_FragColor.rgb = alpha_FragColor.rgb * alpha_FragColor.a + gl_FragColor.rgb*(1.0 - alpha_FragColor.a);"
    "   }"
    "   gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * p_exposure);"
    "   if(enable_sRGB == true)"
	"    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0/2.2));"
	// We put the SSAO outside of the gamma/hdr correction for artistic purpose but it should be before in general
    "   if(enable_SSAO == true) {"
	"       float occlusion  = (texture2D(map_SSAO, gl_TexCoord[0].xy+vec2(0,0)*view_ratio).b*4"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,-1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                       )/16.0;"
    "       gl_FragColor.rgb *= occlusion;"
	"   };"
	/*"   vec2 mapPos = (gl_FragCoord.xy-shadow_shift[0])*shadow_ratio[0];"
    "gl_FragColor *= texture2D(shadow_map_0, mapPos);"*/
    "}";

    m_lightingShader.loadFromMemory(vertexShader.str(),fragShader.str());
}


/*void PBRIsoScene::CompileLightingShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec3 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform sampler2D map_albedo;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_material;" \
    "uniform float p_zPos;" \
    "uniform float view_zoom;"
    "uniform vec2 view_ratio;" \
    "uniform vec2 view_shift;" \
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform float p_isoToCartZFactor;" \
    "uniform vec4 light_ambient;" \
    "uniform int light_nbr;" \
    "uniform int shadow_caster["<<MAX_SHADOW_MAPS<<"];";

    for(int i = 0 ; i < MAX_SHADOW_MAPS ; ++i)
        fragShader<<"uniform sampler2D shadow_map_"<<i<<";";
    fragShader<<
    "uniform vec2 shadow_shift["<<MAX_SHADOW_MAPS<<"];"
    "uniform vec2 shadow_ratio["<<MAX_SHADOW_MAPS<<"];"
    "uniform bool enable_directionalShadows;" \
    "uniform bool enable_dynamicShadows;" \
    "uniform bool enable_SSAO;" \
    "uniform sampler2D map_SSAO;" \
    "uniform bool enable_sRGB;" \
    "varying vec3 v_vertex; "\
    ""
    "float GetShadowCastValue(int curShadowMap, float heightPixel, vec2 shadowPos) {"
    "   vec4 shadowPixel;"
    "   vec2 mapPos = (shadowPos-shadow_shift[curShadowMap])*shadow_ratio[curShadowMap];"
	"   if(curShadowMap == 0)"
	"       shadowPixel = texture2D(shadow_map_0, mapPos);"
	"   float shadowHeight = (0.5-(shadowPixel.r+shadowPixel.g/256.0+shadowPixel.b/65536.0))*"
	<< DEPTH_BUFFER_NORMALISER_INV <<
	";"
    "   return 1.0 - min(1.0,max(0.0, (shadowHeight-heightPixel)*0.05));"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 albedoPixel  = texture2D(map_albedo, gl_TexCoord[0].xy);" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel  = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec4 materialPixel  = texture2D(map_material, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*normalPixel.rgb;"
    "   if(enable_sRGB == true)"
	"       albedoPixel.rgb = pow(albedoPixel.rgb, vec3(2.2));"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*"
    << DEPTH_BUFFER_NORMALISER_INV <<
    ";"
	"   vec3 fragPos = v_vertex*view_zoom+vec3(view_shift.xy,0);"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
    "   gl_FragColor = gl_Color * light_ambient * albedoPixel; "
    "   int curShadowMap = 0;"
    "   for(int i = 0 ; i < light_nbr ; ++i)" \
	"   {" \
	"	    float lighting = 0.0;" \
	"       vec3 lightDirection = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	lightDirection = -gl_LightSource[i].position.xyz;" \
    "           lighting = 1.0/( gl_LightSource[i].constantAttenuation);"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	lightDirection = gl_LightSource[i].position.xyz - fragPos.xyz;" \
	"	    	float dist = length(lightDirection);" \
	"	    	lighting = 1.0/( gl_LightSource[i].constantAttenuation +" \
	"	    					 dist*gl_LightSource[i].linearAttenuation +" \
	"	    				     dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"	    }" \
	"       lightDirection = normalize(lightDirection);"
    "       if((gl_LightSource[i].position.w == 0.0 && enable_directionalShadows == true)"
    "        ||(gl_LightSource[i].position.w != 0.0 && enable_dynamicShadows == true))"
	"       if(curShadowMap < "<<MAX_SHADOW_MAPS<<" && shadow_caster[curShadowMap] == i) {"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec2 shadowPos = gl_FragCoord.xy*view_zoom;"
	"               shadowPos.y += heightPixel*p_isoToCartZFactor;"
	"               vec3 v = vec3((heightPixel*lightDirection.xy/lightDirection.z), 0);"
	"               shadowPos.x -= (v*p_isoToCartMat).x;"
	"               shadowPos.y += (v*p_isoToCartMat).y;"
	"               vec4 shadow_pixel= (0,0,0,0);"
	"               lighting *= (GetShadowCastValue(curShadowMap,heightPixel,shadowPos)*4"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(3,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-3,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,3))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,-3))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,-2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,-2)))/16;"
	"           }"
	"           ++curShadowMap;"
	"       }"
	"       lighting *= max(0.0, dot(direction,lightDirection));"
	"	    lighting *= gl_LightSource[i].diffuse.a;" \
	"	    gl_FragColor.rgb +=  gl_Color.rgb * albedoPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;"
	"   }"
    "   if(enable_SSAO == true) {"
	"       float occlusion  = (texture2D(map_SSAO, gl_TexCoord[0].xy+vec2(0,0)*view_ratio).b*4"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,-1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                       )/16.0;"
    "       gl_FragColor.rgb *= occlusion;"
	"   };"
	//"   gl_FragColor.rgb = gl_FragColor.rgb/(gl_FragColor.rgb+vec3(1.0));"
    //"   gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * .5);"
    "   if(enable_sRGB == true)"
	"    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0/2.2));"
    "}";

    m_lightingShader.loadFromMemory(vertexShader.str(),fragShader.str());
}*/

void PBRIsoScene::CompileSSAOShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec3 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_noise;" \
    "uniform float p_zPos;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform float p_isoToCartZFactor;" \
    "uniform vec2 view_ratio;" \
    "uniform float view_zoom;"
    "uniform vec3 p_samplesHemisphere[16];"
    "varying vec3 v_vertex; "\
    "void main()" \
    "{" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*normalPixel.rgb;"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/255.0+depthPixel.b/65025.0))*"
    << DEPTH_BUFFER_NORMALISER_INV <<
    ";"
	"   vec3 fragPos = v_vertex;"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
    "   float occlusion = 0;"
    "   vec3 rVec = -1.0+2.0*texture2D(map_noise, gl_TexCoord[0].xy);"
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 rayShift = rot * p_samplesHemisphere[i] * 15;"
	"       vec3 screenShift = view_zoom*rayShift*p_isoToCartMat;"
	"       screenShift.y *= -1;"
	"       vec3 screenPos = gl_FragCoord  + screenShift;"
	"       vec3 occl_depthPixel = texture2D(map_depth, (screenPos.xy)*view_ratio);"
	"       float occl_height = (0.5-(occl_depthPixel.r+occl_depthPixel.g/255.0+occl_depthPixel.b/65025.0))*"
	<< DEPTH_BUFFER_NORMALISER_INV <<
	";"
    "       if(occl_height > (fragPos.z+rayShift.z) + .1  "
    "        && occl_height - (fragPos.z+rayShift.z) < 15)"
    "           occlusion += 1.0;"
	"   } "
    "   gl_FragColor.rgb = 1.0-occlusion/12.0;" \
    "   gl_FragColor.a = 1;" \
    "}";

    m_SSAOShader.loadFromMemory(vertexShader.str(),fragShader.str());
}

}

/*const std::string vertexShader = \
    "varying vec3 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";*/


/*
const std::string PBRGeometry_vertShader = \
"#version 330\n"
//"#extension GL_ARB_explicit_attribute_location : require"
//"#extension GL_ARB_explicit_uniform_location : require"
"out vec2 TexCoords;"
"void main()"
"{"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);"
"    TexCoords = aTexCoords;"
"}";


const std::string PBRGeometry_fragShader = \
"#version 330\n"
//"#extension GL_ARB_explicit_attribute_location : require"
//"#extension GL_ARB_explicit_uniform_location : require"
"out vec4 FragColor;"
"in vec2 TexCoords;"
"uniform sampler2D screenTexture;"
"void main()"
"{"
"    FragColor = texture(screenTexture, TexCoords);"
"}";*/

/*const std::string PBRGeometry_vertShader = \
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;"
"layout (location = 1) in vec3 aColor;"
"layout (location = 2) in vec2 aTexCoord;"
"out vec3 glColor;"
"out vec2 texCoords;"
"void main()"
"{"
"    gl_Position = vec4(aPos, 1.0);"
"    glColor = aColor;"
"    texCoords = aTexCoord;"
"}";*/

/*const std::string PBRGeometry_fragShader = \
    "#version 330 compatibility \n"
    "layout (location = 0) out vec4 AlbedoColor;"
    "layout (location = 1) out vec4 NormalColor;"
    "layout (location = 2) out vec4 DepthColor;"
    "layout (location = 3) out vec4 MaterialColor;"
    "uniform sampler2D map_albedo;"  \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform bool enable_normalMap;" \
    "uniform sampler2D map_normal;" \
    "uniform bool enable_materialMap;" \
    "uniform sampler2D map_material;" \
    "uniform float p_roughness;" \
    "uniform float p_metalness;" \
    "uniform float p_translucency;" \
    "uniform mat3 p_normalProjMat;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 albedoPixel = texture2D(map_albedo, gl_TexCoord[0].xy);" \
	"	vec3 direction = vec3(0,0,-1);"
	"   if(enable_normalMap == true){"
	"    direction = -1.0+2.0*texture2D(map_normal, gl_TexCoord[0].xy).rgb;"
	"   }"
	"   direction = direction * p_normalProjMat;"
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "   }"
    "   float zPixel = heightPixel + p_zPos;" \
    "   gl_FragDepth = 1.0 - albedoPixel.a*(0.5+zPixel*0.001);" \
    "   vec4 materialPixel = vec4(p_roughness,p_metalness,p_translucency,albedoPixel.a);"
    "   if(enable_materialMap == true){"
    "       materialPixel = texture2D(map_material, gl_TexCoord[0].xy);"
    "   }"
    "   AlbedoColor = gl_Color*albedoPixel; " \
    "   NormalColor.rgb = 0.5+direction*0.5;" \
    "   NormalColor.a = albedoPixel.a;" \
    "   DepthColor.r = gl_FragDepth;" \
    "   DepthColor.g = (gl_FragDepth - 256.0 * floor(gl_FragDepth / 256.0))*256.0;" \
    "   DepthColor.b = (gl_FragDepth - 65536.0 * floor(gl_FragDepth / (65536.0)))*65536.0;" \
    "   DepthColor.a = albedoPixel.a;" \
    "   MaterialColor = materialPixel; " \
    "}";
*/

 /*   "layout (location = 0) out vec4 FragColor;"
    "layout (location = 1) out vec4 NormalColor;"
    "layout (location = 2) out vec4 DepthColor;"
    "layout (location = 3) out vec4 MaterialColor;"*/


/*const std::string color_fragShader = \
    "uniform sampler2D map_color;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(map_color, gl_TexCoord[0].xy);" \
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "   }"
    "   float zPixel = heightPixel + p_zPos;" \
    "   gl_FragDepth = 1.0 - colorPixel.a*(0.5+zPixel*0.001);" \
    "   gl_FragColor = gl_Color * colorPixel; " \
    "}";*/

/*const std::string depth_fragShader = \
    "uniform sampler2D map_albedo;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(map_albedo, gl_TexCoord[0].xy).a;" \
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "   }"
    "   float zPixel = heightPixel + p_zPos;" \
    "   gl_FragDepth = 1.0 - colorAlpha*(0.5+zPixel*0.001);" \
    "   gl_FragColor.r = gl_FragDepth;" \
    "   gl_FragColor.g = (gl_FragDepth - 256.0 * floor(gl_FragDepth / 256.0))*256.0;" \
    "   gl_FragColor.b = (gl_FragDepth - 65536.0 * floor(gl_FragDepth / (65536.0)))*65536.0;" \
    "   gl_FragColor.a = colorAlpha;" \
    "}";*/

/*const std::string normal_fragShader = \
    "uniform sampler2D map_color;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform bool enable_normalMap;" \
    "uniform sampler2D map_normal;" \
    "uniform mat3 p_normalProjMat;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(map_color, gl_TexCoord[0].xy).a;" \
	"	vec3 direction = vec3(0,0,-1);"
	"   if(enable_normalMap == true){"
	"    direction = -1.0+2.0*texture2D(map_normal, gl_TexCoord[0].xy).rgb;"
	"   }"
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*p_height;"
    "   }"
    "   float zPixel = heightPixel + p_zPos;" \
    "   gl_FragDepth = 1.0 - colorAlpha*(0.5+zPixel*0.001);" \
	"   direction = direction * p_normalProjMat;"
    "   gl_FragColor.rgb = 0.5+direction*0.5;" \
    "   gl_FragColor.a = colorAlpha;" \
    "}";*/

    /** REPLACE 8 SHADOW MAP BY CONSTANT**/
/*const std::string lighting_fragShader = \
    "uniform sampler2D map_albedo;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_material;" \
    "uniform float p_zPos;" \
    "uniform float view_zoom;"
    "uniform vec2 view_ratio;" \
    "uniform vec2 view_shift;" \
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform float p_isoToCartZFactor;" \
    "uniform vec4 light_ambient;" \
    "uniform int light_nbr;" \
    "uniform int shadow_caster[8];"
    "uniform sampler2D shadow_map_0;" \
    "uniform sampler2D shadow_map_1;" \
    "uniform sampler2D shadow_map_2;" \
    "uniform sampler2D shadow_map_3;" \
    "uniform sampler2D shadow_map_4;" \
    "uniform sampler2D shadow_map_5;" \
    "uniform sampler2D shadow_map_6;" \
    "uniform sampler2D shadow_map_7;" \
    "uniform vec2 shadow_shift[8];"
    "uniform vec2 shadow_ratio[8];"
    "uniform bool enable_directionalShadows;" \
    "uniform bool enable_dynamicShadows;" \
    "uniform bool enable_SSAO;" \
    "uniform sampler2D map_SSAO;" \
    "uniform bool enable_sRGB;" \
    "varying vec3 v_vertex; "\
    ""
    "float GetShadowCastValue(int curShadowMap, float heightPixel, vec2 shadowPos) {"
    "   vec4 shadowPixel;"
    "   vec2 mapPos = (shadowPos-shadow_shift[curShadowMap])*shadow_ratio[curShadowMap];"
	"   if(curShadowMap == 0)"
	"       shadowPixel = texture2D(shadow_map_0, mapPos);"
	"   float shadowHeight = (0.5-(shadowPixel.r+shadowPixel.g/256.0+shadowPixel.b/65536.0))*1000;"
    "   return 1.0 - min(1.0,max(0.0, (shadowHeight-heightPixel)*0.05));"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 albedoPixel  = texture2D(map_albedo, gl_TexCoord[0].xy);" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel  = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec4 materialPixel  = texture2D(map_material, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*normalPixel.rgb;"
    "   if(enable_sRGB == true)"
	"       albedoPixel.rgb = pow(albedoPixel.rgb, vec3(2.2));"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*1000;"
	"   vec3 fragPos = v_vertex*view_zoom+vec3(view_shift.xy,0);"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
    "   gl_FragColor = gl_Color * light_ambient * albedoPixel; "
    "   int curShadowMap = 0;"
    "   for(int i = 0 ; i < light_nbr ; ++i)" \
	"   {" \
	"	    float lighting = 0.0;" \
	"       vec3 lightDirection = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	lightDirection = -gl_LightSource[i].position.xyz;" \
    "           lighting = 1.0/( gl_LightSource[i].constantAttenuation);"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	lightDirection = gl_LightSource[i].position.xyz - fragPos.xyz;" \
	"	    	float dist = length(lightDirection);" \
	"	    	lighting = 1.0/( gl_LightSource[i].constantAttenuation +" \
	"	    					 dist*gl_LightSource[i].linearAttenuation +" \
	"	    				     dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"	    }" \
	"       lightDirection = normalize(lightDirection);"
    "       if((gl_LightSource[i].position.w == 0.0 && enable_directionalShadows == true)"
    "        ||(gl_LightSource[i].position.w != 0.0 && enable_dynamicShadows == true))"
	"       if(curShadowMap < 8 && shadow_caster[curShadowMap] == i) {"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec2 shadowPos = gl_FragCoord.xy*view_zoom;"
	"               shadowPos.y += heightPixel*p_isoToCartZFactor;"
	"               vec3 v = vec3((heightPixel*lightDirection.xy/lightDirection.z), 0);"
	"               shadowPos.x -= (v*p_isoToCartMat).x;"
	"               shadowPos.y += (v*p_isoToCartMat).y;"
	"               vec4 shadow_pixel= (0,0,0,0);"
	"               lighting *= (GetShadowCastValue(curShadowMap,heightPixel,shadowPos)*4"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(3,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-3,0))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,3))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(0,-3))*2"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(-2,-2))"
    "                            +GetShadowCastValue(curShadowMap,heightPixel,shadowPos+vec2(2,-2)))/16;"
	"           }"
	"           ++curShadowMap;"
	"       }"
	"       lighting *= max(0.0, dot(direction,lightDirection));"
	"	    lighting *= gl_LightSource[i].diffuse.a;" \
	"	    gl_FragColor.rgb +=  gl_Color.rgb * albedoPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;"
	"   }"
    "   if(enable_SSAO == true) {"
	"       float occlusion  = (texture2D(map_SSAO, gl_TexCoord[0].xy+vec2(0,0)*view_ratio).b*4"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,-1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                       )/16.0;"
    "       gl_FragColor.rgb *= occlusion;"
	"   };"
	//"   gl_FragColor.rgb = gl_FragColor.rgb/(gl_FragColor.rgb+vec3(1.0));"
    //"   gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * .5);"
    "   if(enable_sRGB == true)"
	"    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0/2.2));"
    "}";*/

/*
const std::string SSAO_fragShader = \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_noise;" \
    "uniform float p_zPos;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform float p_isoToCartZFactor;" \
    "uniform vec2 view_ratio;" \
    "uniform float view_zoom;"
    "uniform vec3 p_samplesHemisphere[16];"
    "varying vec3 v_vertex; "\
    "void main()" \
    "{" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*normalPixel.rgb;"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*1000;"
	"   vec3 fragPos = v_vertex;"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
    "   float occlusion = 0;"
    "   vec3 rVec = -1.0+2.0*texture2D(map_noise, gl_TexCoord[0].xy);"
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 rayShift = rot * p_samplesHemisphere[i] * 20;"
	"       vec3 screenShift = view_zoom*rayShift*p_isoToCartMat;"
	"       screenShift.y *= -1;"
	"       vec3 screenPos = gl_FragCoord  + screenShift;"
	"       vec3 occl_depthPixel = texture2D(map_depth, (screenPos.xy)*view_ratio);"
	"       float occl_height = (0.5-(occl_depthPixel.r+occl_depthPixel.g/256.0+occl_depthPixel.b/65536.0))*1000;"
    "       if(occl_height > (fragPos.z+rayShift.z) + 1  "
    "        && occl_height - (fragPos.z+rayShift.z) < 20)"
    "           occlusion += 1.0;"
	"   } "
    "   gl_FragColor.rgb = 1.0-occlusion/12.0;" \
    "   gl_FragColor.a = 1;" \
    "}";

*/



