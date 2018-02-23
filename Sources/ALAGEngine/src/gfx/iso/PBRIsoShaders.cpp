#include "ALAGE/gfx/iso/PBRIsoScene.h"

#include <sstream>
#include <iostream>

namespace alag
{

void PBRIsoScene::CompileDepthShader()
{
    std::ostringstream fragShader;

    fragShader<<
    "uniform bool enable_albedoMap;"
    "uniform sampler2D map_albedo;" \
    "uniform bool enable_depthColor;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   const vec3 All33 = vec3(-.33);"
    "   const vec4 All25 = vec4(-.25);"
    "   const vec2 All255 = vec2(0.00392156862,1.0);"
    "   float colorAlpha = 1.0;" \
    "   if(enable_albedoMap == true){"
    "       colorAlpha = texture2D(map_albedo, gl_TexCoord[0].xy).a;}"
    "   float heightPixel = 0; "
    "   if(enable_depthMap == true){"
    "        vec3 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy).rgb;" \
    "       heightPixel = dot(All33, depthPixel);"
    "   } else if(enable_depthColor == true) {"
    "       heightPixel = dot(All25, gl_Color);"
    "   }"
    "   float zPixel = heightPixel*p_height - p_zPos;" \
    "   float nzPixel = zPixel-0.5;"
    "   gl_FragDepth =   colorAlpha*nzPixel + 1.0;" \
    "   float r = floor(gl_FragDepth * 255.0);"
    "   float g = floor((gl_FragDepth*255.0 -  r)*255.0);"
    "   float b = floor((gl_FragDepth*65025.0 -  g - r*255.0)*255.0);"
    "   gl_FragColor = vec4(r,g,b,1.0)*All255.xxxy;"
    "}";

    m_depthShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
}

void PBRIsoScene::CompileDepthCopierShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec2 VaryingTexCoord0; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    VaryingTexCoord0 = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform sampler2D map_albedo;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_material;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_depthTester;" \
    "uniform vec2 view_ratio;"
    "varying vec2 VaryingTexCoord0; "\
    "void main()" \
    "{" \
    "       vec4 depthPixel = texture2D(map_depth, VaryingTexCoord0);" \
    "       float depth = 1.0;"
    "       if(depthPixel.a != 0)"
    "           depth =((depthPixel.b*"<<1.0/255.0<<"+depthPixel.g)*"<<1.0/255.0<<"+depthPixel.r);"
     "      vec4 depthTestPixel = texture2D(map_depthTester, gl_FragCoord.xy*view_ratio);" \
     "      float depthTest = 1.0;"
    "       if(depthTestPixel.a != 0)"
     "          depthTest = ((depthTestPixel.b*"<<1.0/255.0<<"+depthTestPixel.g)*"<<1.0/255.0<<"+depthTestPixel.r);"
     "      if(depth <= depthTest){"
    "           gl_FragData["<<PBRAlbedoScreen<<"] = texture2D(map_albedo, VaryingTexCoord0); " \
    "           gl_FragData["<<PBRNormalScreen<<"] = texture2D(map_normal, VaryingTexCoord0);" \
    "           gl_FragData["<<PBRDepthScreen<<"] = depthPixel;"
    "           gl_FragData["<<PBRMaterialScreen<<"] = texture2D(map_material, VaryingTexCoord0); "
    "           gl_FragDepth = depth;"
    "       }else {"
    "         gl_FragDepth = depthTest;"
    "         gl_FragData["<<PBRAlbedoScreen<<"] = vec4(0,0,0,0);"
    "         gl_FragData["<<PBRNormalScreen<<"] = vec4(0,0,0,0);"
    "         gl_FragData["<<PBRDepthScreen<<"] = depthTestPixel;"
    "         gl_FragData["<<PBRMaterialScreen<<"] = vec4(0,0,0,0);"
    "       }"
    "}";

    m_depthCopierShader.loadFromMemory(vertexShader.str(), fragShader.str());
}

void PBRIsoScene::CompilePBRGeometryShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec2 VaryingTexCoord0; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    VaryingTexCoord0 = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    /*"#version 330 compatibility \n"
    "layout (location = 0) out vec4 AlbedoColor;"
    "layout (location = 1) out vec4 NormalColor;"
    "layout (location = 2) out vec4 DepthColor;"
    "layout (location = 3) out vec4 MaterialColor;"*/
    "uniform bool p_alpha_pass;"
    "uniform sampler2D map_albedo;"  \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform bool enable_normalMap;" \
    "uniform sampler2D map_normal;" \
    "uniform bool enable_materialMap;" \
    "uniform sampler2D map_material;" \
  /*  "uniform sampler2D map_depthTester;" \
    "uniform bool enable_depthTesting;"*/
    "uniform float p_roughness;" \
    "uniform float p_metalness;" \
    "uniform float p_translucency;" \
    "uniform mat3 p_normalProjMat;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
  //  "uniform vec2 view_ratio;"
    ""
    "varying vec2 VaryingTexCoord0; "\
    ""
    "void main()" \
    "{" \
    "   const vec3 All33 = vec3(-.33);"
    "   const vec2 All255 = vec2(0.00392156862,1.0);"
    "   vec4 albedoPixel = gl_Color*texture2D(map_albedo, VaryingTexCoord0);" \
    "   if((p_alpha_pass == true && albedoPixel.a > .2 && albedoPixel.a < .9) "
    "   || (p_alpha_pass == false && albedoPixel.a >= .9))"
    "   {"
    "       float heightPixel = 0; "
    "       if(enable_depthMap == true){"
    "           vec3 depthPixel = texture2D(map_depth, VaryingTexCoord0).rgb;" \
    "           heightPixel = dot(All33, depthPixel);"
    "       }"
    "       float zPixel = heightPixel*p_height - p_zPos +0.5;" \
     /*"       float depthTest = 1.0;"
     "       if(enable_depthTesting == true){"
     "           vec3 depthTestPixel = texture2D(map_depthTester, gl_FragCoord.xy*view_ratio).rgb;" \
     "           depthTest = ((depthTestPixel.b*"<<1.0/255.0<<"+depthTestPixel.g)*"<<1.0/255.0<<"+depthTestPixel.r);"
     "       }"
     "      if(enable_depthTesting == false || zPixel <= depthTest){"*/
	"	        vec4 direction = vec4(0,0,1.0,1.0);"
	"           if(enable_normalMap == true){"
	"               direction.xyz = 2.0*texture2D(map_normal, VaryingTexCoord0).rgb-1.0;"
	"           }"
	"           direction.xyz = direction.xyz * p_normalProjMat;"
    "           gl_FragDepth = zPixel;"
    "           float r = floor(gl_FragDepth * 255.0);"
    "           float g = floor((gl_FragDepth*255.0 -  r)*255.0);"
    "           float b = floor((gl_FragDepth*65025.0 -  g - r*255.0)*255.0);"
    "           vec4 materialPixel = vec4(p_roughness,p_metalness,p_translucency,1.0);"
    "           if(enable_materialMap == true){"
    "               materialPixel.rgb = texture2D(map_material, VaryingTexCoord0).rgb;"
    "           }"
    "           gl_FragData["<<PBRAlbedoScreen<<"] = albedoPixel; " \
    "           gl_FragData["<<PBRNormalScreen<<"] = (direction*0.5+0.5);" \
    "           gl_FragData["<<PBRDepthScreen<<"] = vec4(r,g,b,1.0)*All255.xxxy;"
    "           gl_FragData["<<PBRMaterialScreen<<"] = materialPixel; "
   /* "       }else {"
    "         gl_FragDepth = zPixel;"
    "         gl_FragData["<<PBRAlbedoScreen<<"] = vec4(0,0,0,0);"
    "         gl_FragData["<<PBRNormalScreen<<"] = vec4(0,0,0,0);"
    "         gl_FragData["<<PBRDepthScreen<<"] = vec4(0,0,0,0);"
    "       gl_FragData["<<PBRMaterialScreen<<"] = vec4(0,0,0,0);"
    "       }"*/
    "   } else {"
    "       gl_FragDepth = 1.0;"
    "       gl_FragData["<<PBRAlbedoScreen<<"] = vec4(0,0,0,0);"
    "       gl_FragData["<<PBRNormalScreen<<"] = vec4(0,0,0,0);"
    "       gl_FragData["<<PBRDepthScreen<<"] = vec4(0,0,0,0);"
    "       gl_FragData["<<PBRMaterialScreen<<"] = vec4(0,0,0,0);"
    "   }"
    "}";

   // m_PBRGeometryShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
    m_PBRGeometryShader.loadFromMemory(vertexShader.str(),fragShader.str());
}


/** Lighting inspired by tutorials on https://learnopengl.com **/

void PBRIsoScene::CompileLightingShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
    "varying vec3 v_vertex; "\
    "varying vec2 VaryingTexCoord0; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    VaryingTexCoord0 = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform sampler2D map_albedo;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_material;" \
    "uniform sampler2D map_noise;" \
    "uniform sampler2D map_brdflut;" \
    "uniform sampler2D map_environmental;" \
    "uniform float p_zPos;" \
    "uniform float view_zoom;"
    "uniform vec2 view_ratio;" \
    "uniform vec2 view_shift;" \
    "uniform vec3 view_pos;" \
   // "uniform vec3 view_direction;"
    "uniform mat3 p_cartToIso2DProjMat;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform float p_isoToCartZFactor;"
   // "uniform float p_exposure;"
    "uniform vec4 light_ambient;" \
    "uniform int light_nbr;" \
    "uniform float shadow_caster["<<MAX_SHADOW_MAPS<<"];";
    for(int i = 0 ; i < MAX_SHADOW_MAPS ; ++i)
        fragShader<<"uniform sampler2D shadow_map_"<<i<<";";
    fragShader<<
    "uniform vec2 shadow_shift["<<MAX_SHADOW_MAPS<<"];"
    "uniform vec2 shadow_ratio["<<MAX_SHADOW_MAPS<<"];"
    "uniform bool enable_directionalShadows;" \
    "uniform bool enable_dynamicShadows;" \
    "uniform bool enable_bloom;"
    //"uniform bool enable_SSAO;"
    //"uniform sampler2D map_SSAO;"
    "uniform bool enable_sRGB;" \
    ""
    "varying vec3 v_vertex; "\
    "varying vec2 VaryingTexCoord0; "\
    ""
    "float GetShadowCastValue(int curShadowMap, float depth, vec2 shadowPos) {"
    "   vec3 shadowPixel;"
    "   vec2 mapPos = (shadowPos-shadow_shift[curShadowMap])*shadow_ratio[curShadowMap];";
    for(int i = 0 ; i < MAX_SHADOW_MAPS ; ++i)
        fragShader<<"if(curShadowMap == "<<i<<"){"
        <<"shadowPixel = texture2D(shadow_map_"<<i<<", mapPos).rgb;}";
    fragShader<<
	"   float shadowDepth = ((shadowPixel.b*"<<1.0/255.0<<"+shadowPixel.g)*"<<1.0/255.0<<"+shadowPixel.r);"
    "   return 1.0 - min(1.0,max(0.0, (depth-shadowDepth)*5000.0));"
    "}"
    ""
    "vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)"
    "{"
    "    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);"
    "}   "
    ""
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)"
    "{"
    "    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);"
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
    "    denom = "<<PI<<" * denom * denom;"
    "    return nom / denom;"
    "}"
    "float GeometrySchlickGGX(float NdotV, float roughness)"
    "{"
    "    float r = (roughness + 1.0);"
    "    float k = (r*r) *"<< 1.0/ 8.0<<";"
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
    "   const vec3 constantList = vec3(1.0, 0.0, -1.0);"
    "   vec4 albedoPixel    = texture2D(map_albedo, VaryingTexCoord0);" \
    "   if(albedoPixel.a > .1) {"
    "   vec3 depthPixel     = texture2D(map_depth, VaryingTexCoord0).rgb;" \
    "   float depth = ((depthPixel.b*"<<1.0/255.0<<"+depthPixel.g)*"<<1.0/255.0<<"+depthPixel.r);"
   /* "   float depthTest = 1.0;"
    "   if(enable_depthTesting == true){"
    "       vec3 depthTestPixel = texture2D(map_depthTester, gl_TexCoord[0].xy).rgb;" \
    "       depthTest = ((depthTestPixel.b*"<<1.0/255.0<<"+depthTestPixel.g)*"<<1.0/255.0<<"+depthTestPixel.r);"
    "   }"*/
  //  "   if(enable_depthTesting == false || depth <= depthTest){"
    "   vec2 rVec = 2*texture2D(map_noise, VaryingTexCoord0*view_ratio/4.0).rg-1.0;"
    "   gl_FragData[0].a = albedoPixel.a; "
    "   float heightPixel = "<<0.5*PBRTextureAsset::DEPTH_BUFFER_NORMALISER_INV<<
    "           -depth*"<< PBRTextureAsset::DEPTH_BUFFER_NORMALISER_INV <<";"
    "   bool enable_alpha = false;"
    "   vec3 direction      = normalize(2.0*texture2D(map_normal, VaryingTexCoord0).rgb-1.0);"
    "   vec3 materialPixel  = texture2D(map_material, VaryingTexCoord0).rgb;" \
    "   if(enable_sRGB == true)"
	"       albedoPixel.rgb = pow(albedoPixel.rgb, vec3(2.2));"
    ""
	"   vec3 fragPos;"
	"   fragPos.xy = v_vertex.xy*view_zoom+view_shift.xy;"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = vec3(fragPos.xy,0.0)*p_cartToIso2DProjMat + vec3(0.0,0.0,heightPixel);"
	""
    "   vec3 viewDirection = normalize(view_pos - fragPos);"
 //   "   vec3 viewDirection = view_direction;"
  //  "   vec3 viewDirection = vec3("<<cos(m_viewAngle.xyAngle*PI/180)*cos(m_viewAngle.zAngle*PI/180)<<","
    //                                <<sin(m_viewAngle.xyAngle*PI/180)*cos(m_viewAngle.zAngle*PI/180)<<","
      //                              <<sin(m_viewAngle.zAngle*PI/180)<<");"
    "   vec3 surfaceReflection0 = vec3(0.04);"
    "   surfaceReflection0 = mix(surfaceReflection0, albedoPixel.rgb, materialPixel.g);"
    "   vec3 FAmbient = fresnelSchlickRoughness(max(dot(direction, viewDirection), 0.0), surfaceReflection0, materialPixel.r);"
    "   vec3 kSAmbient = FAmbient;"
    "   vec3 kDAmbient = (1.0 - kSAmbient)*(1.0 - materialPixel.g);"
    "   vec3 ambientLighting = light_ambient.rgb * light_ambient.a;"
    "   vec3 irradianceAmbient = ambientLighting;"
    "   vec3 reflectionView = reflect(-viewDirection, direction);"
    "   vec3 reflectionColor = ambientLighting;"
    "   if(reflectionView.z < 0){"
	"       vec2 groundPos = gl_FragCoord.xy*view_zoom;"
	"       vec3 v = vec3((heightPixel/reflectionView.z)*reflectionView.xy,0.0);"
	"       groundPos.xy += (heightPixel*p_isoToCartZFactor)*constantList.yx "
	"                                  +(v*p_isoToCartMat).xy * constantList.zx;"
    "       vec3 groundColor = max(constantList.yyy,texture2DLod(map_environmental, groundPos*view_ratio,materialPixel.r*15.0).rgb) *50.0/(1.0+length(v)+abs(heightPixel));"
    "       reflectionColor = mix(groundColor, ambientLighting, reflectionView.z+1.0); "
    "   }"
    "   vec2 envBRDF  = texture2D(map_brdflut, vec2(max(dot(direction, viewDirection), 0.0), materialPixel.r)).rg;"
    "   vec3 specularAmbient = reflectionColor* (FAmbient * envBRDF.x + envBRDF.y);"
    "   gl_FragData[0].rgb = (albedoPixel.rgb * irradianceAmbient + specularAmbient); "
    ""
    ""
    "   int curShadowMap = 0;"
    "   for(int i = 0 ; i < light_nbr ; ++i)" \
	"   {" \
	"	    float attenuation = 0.0;" \
	"       vec3 lightDirection = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	lightDirection = -gl_LightSource[i].position.xyz;" \
    "           attenuation = 1.0;"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	lightDirection = gl_LightSource[i].position.xyz - fragPos.xyz;" \
	"	    	float dist = length(lightDirection)*0.01;" \
	"           float dr = dist*gl_LightSource[i].constantAttenuation;"
	"           float sqrtnom = 1.0 - dr*dr*dr*dr;"
    "           if(sqrtnom >= 0)"
	//"               attenuation = saturate(sqrtnom*sqrtnom/(dist*dist+1.0));"
	"               attenuation = clamp(sqrtnom*sqrtnom/(dist*dist+1.0),0.0,1.0);"
	"	    }" \
    "       if((gl_LightSource[i].position.w == 0.0 && enable_directionalShadows == true)"
    "        ||(gl_LightSource[i].position.w != 0.0 && enable_dynamicShadows == true))"
	"       if(curShadowMap < "<<MAX_SHADOW_MAPS<<" && shadow_caster[curShadowMap] == i) {"
	"               vec2 shadowPos = gl_FragCoord.xy*view_zoom;"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec3 v = vec3((heightPixel/lightDirection.z)*lightDirection.xy,0.0);"
	"               shadowPos.xy += (heightPixel*p_isoToCartZFactor)*constantList.yx "
	"                                  +(v*p_isoToCartMat).xy * constantList.zx;"
	"           } else {"
	"               shadowPos.y += heightPixel*p_isoToCartZFactor;"
	"           }"
	"               float shadowing = (GetShadowCastValue(curShadowMap,depth,shadowPos)*2.0"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(2.0,-2.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-2.0,2.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(2.0,2.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-2.0,-2.0)*rVec))*"<<1.0/6.0<<";"
    "               if(shadowing > .2 && shadowing < .8) {"
    "                   shadowing =  shadowing * 0.5"
    "                            +(GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(4.0,0.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-4.0,0.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(0.0,4.0)*rVec)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(0.0,-4.0)*rVec))*"<<1.0/8.0<<"; "
    "               }"
    "               attenuation *= shadowing;"
	"           ++curShadowMap;"
	"       }"
	"       lightDirection = normalize(lightDirection);"
    "       vec3 halfwayVector = normalize(viewDirection + lightDirection);"
    "       vec3 radiance = attenuation*gl_LightSource[i].diffuse.rgb; "
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
	"	    gl_FragData[0].rgb += (kD * albedoPixel.rgb *"<<1.0/PI<<" + specular) * radiance * NdotL;"
	"       float t = materialPixel.b;"
	"	    gl_FragData[0].rgb -= (albedoPixel.rgb*"<<1.0/PI<<") * radiance * min(dot(direction, lightDirection), 0.0)*t;"
	""
	"   }"
	"   float brightness = dot(gl_FragData[0].rgb, vec3(0.2126, 0.7152, 0.0722))*albedoPixel.a;"
	"   if(enable_bloom == true && brightness > 1.5)"
	"       gl_FragData[1] = gl_FragData[0];"
	"   else"
    "       gl_FragData[1] = vec4(0.0,0.0,0.0,albedoPixel.a);"
	"} else {"
    "   gl_FragData[0].a = 0; "
    "   if(enable_bloom == true) "
    "   gl_FragData[1].a = 0; }"
    "}";

    m_lightingShader.loadFromMemory(vertexShader.str(),fragShader.str());
}

void PBRIsoScene::CompileHDRBloomShader()
{
    std::ostringstream fragShader;

    fragShader<<
    "uniform sampler2D texture;"
    "uniform bool enable_bloom;"
    "uniform sampler2D bloom_map;"
    "uniform bool enable_sRGB;"
    "uniform float p_exposure;"
 //   "uniform bool enable_SSAO;"
  //  "uniform sampler2D map_SSAO;"
   // "uniform vec2 view_ratio;"
    "void main()"
    "{"
    "   gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);" \
    ""
    "   if(enable_bloom == true){"
    "       gl_FragColor.rgb += texture2D(bloom_map, gl_TexCoord[0].xy).rgb*0.5;"
    "   }"
    ""
    "   gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * p_exposure);"
   "   if(enable_sRGB == true)"
	"       gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3("<<1.0/2.2<<"));"
	// We put the SSAO outside of the gamma/hdr correction for artistic purpose but it should be before in general
   // "   if(enable_SSAO == true) {"
	//"       float occlusion  = (texture2D(map_SSAO, gl_TexCoord[0].xy+vec2(0,0)*view_ratio).r);"
	/*"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,0))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,0))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,1))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,-1))*view_ratio).b)*"<<1.0/6.0<<";"
	"       if(occlusion > .2 && occlusion < .8) {"
    "           occlusion = occlusion*0.75"
	"                      +(texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,1))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,-1))*view_ratio).b"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b"
	"                       )*"<<1.0/16<<";"
	"       }"*/
	/*"       float occlusion  = (texture2D(map_SSAO, gl_TexCoord[0].xy+vec2(0,0)*view_ratio).b*4"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,0))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(0,-1))*view_ratio).b*2"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(-1,-1))*view_ratio).b*1"
	"                      +texture2D(map_SSAO, gl_TexCoord[0].xy+(vec2(1,-1))*view_ratio).b*1"
	"                       )/16.0;"*/
    //"       gl_FragColor.rgb *= occlusion;"
	//"   }"
    "}";

    m_HDRBloomShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
    m_blurShader.setUniform("texture",sf::Shader::CurrentTexture);

}

void PBRIsoScene::CompileSSAOShader()
{
    std::ostringstream fragShader/*, vertexShader*/;

    /*vertexShader<<
    "varying vec2 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xy; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";*/

    fragShader<<
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
    "uniform sampler2D map_noise;" \
    "uniform mat3 p_isoToCartMat;" \
    "uniform vec2 view_ratio;" \
    "uniform float view_zoom;"
    "uniform vec3 p_samplesHemisphere[16];"
    "void main()" \
    "{" \
    "   const vec3 constantList = vec3(1.0, 0.0, -1.0);"
    "   vec3 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy).rgb;" \
    "   vec3 depthPixel = texture2D(map_depth, gl_TexCoord[0].xy).rgb;" \
    "   vec3 direction = 2.0*normalPixel.rgb-1.0;"
    "   float heightPixel = (depthPixel.b*"<<1.0/255.0<<"+depthPixel.g)*"<<1.0/255.0<<"+depthPixel.r;"
    "   float occlusion = 12.0;"
    "   vec3 rVec = 2.0*texture2D(map_noise, gl_TexCoord[0].xy*view_ratio/4.0).rgb-1.0;"
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 rayShift = (rot * p_samplesHemisphere[i]) ;"
	"       vec2 screenShift = (view_zoom* 15.0)*(rayShift*p_isoToCartMat).xy;"
	"       rayShift.z *= "<<15.0*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<";"
	//"       screenShift.y *= -1;"
	"       vec2 screenPos = gl_FragCoord.xy  + screenShift * constantList.xz;"
	"       vec3 occl_depthPixel = texture2D(map_depth, screenPos*view_ratio).rgb;"
	"       float occl_height = (occl_depthPixel.b*"<<1.0/255.0<<"+occl_depthPixel.g)*"<<1.0/255.0<<"+occl_depthPixel.r;"
    "       if(occl_height < (heightPixel-rayShift.z) - "<<0.1*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<"  "
    "        && occl_height - (heightPixel-rayShift.z) > "<<-15*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<")"
    "           --occlusion;"
	"   } "
    "   gl_FragColor.rgb = constantList.xxx*pow(occlusion*"<<1.0/12.0<<","<<SSAO_STRENGTH<<");" \
    "   gl_FragColor.a = 1;" \
    "}";

    m_SSAOShader.loadFromMemory(/*vertexShader.str(),*/fragShader.str(),sf::Shader::Fragment);
}


void PBRIsoScene::CompileBlurShader()
{
     std::ostringstream fragShader;

    fragShader<<
    "uniform sampler2D texture;"
    "uniform vec2 offset;"
    ""
   // "uniform float offset_shift[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );"
    //"uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );"
    ""
    "void main()"
    "{"
    ""
   /* "   gl_FragColor = texture2D(texture, gl_TexCoord[0].xy + offset * 1.0) * weight[0];"
    "   for(int i = 1 ; i < 3 ; ++i){"
    "       gl_FragColor += texture2D(texture, gl_TexCoord[0].xy + offset * offset_shift[i]) * weight[i];"
    "       gl_FragColor += texture2D(texture, gl_TexCoord[0].xy - offset * offset_shift[i]) * weight[i];"
    "   }"*/
    ""
  /*  "    gl_FragColor =  gl_Color * "
    "                   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.06 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.75)	* 0.09 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.5)	* 0.12 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.25)	* 0.15 + "
    "                    texture2D(texture, gl_TexCoord[0].xy)	* 0.16 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0) 	* 0.06 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.75)	* 0.09 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.5)	* 0.12 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.25)	* 0.15 ); "*/
   "    gl_FragColor =  gl_Color * "
	"			   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.000003 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.8)	* 0.000229 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.6)	* 0.005977 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.4)	* 0.060598 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.2)	* 0.24173 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 0.382925 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0)	* 0.000003 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.8)	* 0.000229 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.6)	* 0.005977 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.4)	* 0.060598 + "
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 0.24173); "
   /* "    gl_FragColor =  gl_Color * "
	"			   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.0093 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.8)	* 0.028002 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.6)	* 0.065984 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.4)	* 0.121703 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.2)	* 0.175713 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 0.198596 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0)	* 0.0093 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.8)	* 0.028002 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.6)	* 0.065984 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.4)	* 0.121703 + "
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 0.175713); "*/
   /* "    gl_FragColor =  gl_Color * "
	"			   ("
	//"               texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.000001 + "
	//"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.9)	* 0.000001 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.8)	* 0.000078 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.7)	* 0.000489 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.6)	* 0.002403 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.5)	* 0.009245 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.4)	* 0.027835 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.3)	* 0.065591 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.2)	* 0.120978 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.1)	* 0.174666 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 	0.197413 + "
	//"			    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0)	* 0.000001 + "
	//"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.9)	* 0.000001 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.8)	* 0.000078 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.7)	* 0.000489 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.6)	* 0.002403 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.5)	* 0.009245 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.4)	* 0.027835 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.3)	* 0.065591 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 0.120978 + "
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.1)	* 0.174666);"*/
    /*"    gl_FragColor =  gl_Color * "
	"			   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.011254 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.9)	* 0.016436 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.8)	* 0.023066 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.7)	* 0.031105 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.6)	* 0.040306 + "
	"			    texture2D(texture, gl_TexCoord[0].xy + offset * 0.5)	* 0.050187 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.4)	* 	0.060049 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.3)	* 0.069041 + "
	"				texture2D(texture, gl_TexCoord[0].xy + offset * 0.2)	* 	0.076276 + "
	"    			texture2D(texture, gl_TexCoord[0].xy + offset * 0.1)	* 0.080977	 + "
	"				texture2D(texture, gl_TexCoord[0].xy)	* 		0.082607 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0)	* 0.011254 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.9)	* 0.016436 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.8)	* 0.023066 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.7)	* 0.031105 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.6)	* 0.040306 + "
	"			    texture2D(texture, gl_TexCoord[0].xy - offset * 0.5)	* 0.050187 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.4)	* 	0.060049 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.3)	* 0.069041 + "
	"				texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 	0.076276 + "
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.1)	* 0.080977); "*/
    "}";

    m_blurShader.loadFromMemory(fragShader.str(),sf::Shader::Fragment);
    m_blurShader.setUniform("texture",sf::Shader::CurrentTexture);
}
}


