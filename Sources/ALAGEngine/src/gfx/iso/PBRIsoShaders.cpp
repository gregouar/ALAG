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
    "uniform bool enable_depthTesting;"
    "uniform vec2 view_ratio;"
    "varying vec2 VaryingTexCoord0; "\
    "void main()" \
    "{" \
    "       vec4 depthPixel = texture2D(map_depth, VaryingTexCoord0);" \
    "       float depth = 1.0;"
    "       if(depthPixel.a != 0)"
    "           depth = ((depthPixel.b*"<<1.0/255.0<<"+depthPixel.g)*"<<1.0/255.0<<"+depthPixel.r);"
     "      vec4 depthTestPixel = vec4(0.0);" \
     "      float depthTest = 1.0;"
     "      if(enable_depthTesting == true){"
     "          depthTestPixel = texture2D(map_depthTester, gl_FragCoord.xy*view_ratio);"
    "           if(depthTestPixel.a != 0.0)"
     "           depthTest = ((depthTestPixel.b*"<<1.0/255.0<<"+depthTestPixel.g)*"<<1.0/255.0<<"+depthTestPixel.r);"
     "      } "
     "      if(enable_depthTesting == false || (enable_depthTesting && depth <= depthTest)){"
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
    "uniform bool enable_parallax;" \
  /*  "uniform sampler2D map_depthTester;" \
    "uniform bool enable_depthTesting;"*/
    "uniform float p_roughness;" \
    "uniform float p_metalness;" \
    "uniform float p_translucency;" \
    "uniform mat3 p_normalProjMat;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    ""
    "uniform vec3 view_direction;"
    "uniform vec2 texture_size;"
  //  "uniform vec2 view_ratio;"
    ""
    "varying vec2 VaryingTexCoord0; "\
    ""
    "const vec3 All33 = vec3(-.33);"
    ""
    "vec2 Parallax(vec2 coord)"
    "{"
    "   float total_height = p_height*"<<PBRTextureAsset::DEPTH_BUFFER_NORMALISER_INV<<";"
    "   vec3 depthPixel = texture2D(map_depth, coord).rgb;"
    "   vec3 oldHeight = 0.0;"
    "   float layerHeight = 0.0;"
    "   float curHeight = -dot(All33, depthPixel);"
    "   vec2 p = vec2(0,0);"
    "   while(curHeight > layerHeight) {"
    "       p -= view_direction.xy *  "<<1.0/NBR_PARALLAX_STEPS<<" * total_height;"
    "       depthPixel = texture2D(map_depth, coord - p/texture_size).rgb;"
    "       oldHeight = curHeight;"
    "       curHeight = -dot(All33, depthPixel);"
    "       layerHeight += "<<1.0/NBR_PARALLAX_STEPS<<";"
    "   }"
    ""
    "   float delta = oldHeight - curHeight;"
    "   float weight = 1.0 - "<<NBR_PARALLAX_STEPS<<"*(oldHeight - layerHeight + "<<1.0/NBR_PARALLAX_STEPS<<")/(1.0+delta*"<<NBR_PARALLAX_STEPS<<");"
    "   p += view_direction.xy *  "<<1.0/NBR_PARALLAX_STEPS<<" * total_height * weight;"
    ""
    "   return coord - p / texture_size;"
    "}"
    ""
    "void main()" \
    "{" \
    "   const vec2 All255 = vec2(0.00392156862,1.0);"
    "   vec2 texCoord = VaryingTexCoord0;"
    "   if(enable_parallax == true && enable_depthMap == true){"
    "       texCoord = Parallax(texCoord);}"
    //"   if(texCoord.x > 2.0 || texCoord.x < 0.0 || texCoord.y > 2.0 || texCoord.y < 0.0) discard;"
    "   vec4 albedoPixel = gl_Color*texture2D(map_albedo, texCoord);" \
    "   if((p_alpha_pass == true && albedoPixel.a > .1 && albedoPixel.a < .99) "
    "   || (p_alpha_pass == false && albedoPixel.a >= .99))"
    "   {"
    "       float heightPixel = 0.0; "
    "       if(enable_depthMap == true){"
    "           vec3 depthPixel = texture2D(map_depth, texCoord).rgb;" \
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
	"               direction.xyz = 2.0*texture2D(map_normal, texCoord).rgb-1.0;"
	"           }"
	"           direction.xyz = direction.xyz * p_normalProjMat;"
    "           gl_FragDepth = zPixel;"
    "           float r = floor(gl_FragDepth * 255.0);"
    "           float g = floor((gl_FragDepth*255.0 -  r)*255.0);"
    "           float b = floor((gl_FragDepth*65025.0 -  g - r*255.0)*255.0);"
    "           vec4 materialPixel = vec4(p_roughness,p_metalness,p_translucency,1.0);"
    "           if(enable_materialMap == true){"
    "               materialPixel.rgb = texture2D(map_material, texCoord).rgb;"
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
    "       discard;"
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
    "uniform bool enable_SSR;"
    "uniform sampler2D map_brdflut;" \
    "uniform vec2 SSR_map_shift;" \
    "uniform sampler2D map_SSRenv;" \
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
    "const vec2 invAtan = vec2("<<1.0/(2*PI)<<", "<<1.0/PI<<");"
    "vec2 SampleSphericalMap(vec3 v)"
    "{"
    "    vec2 uv = vec2(atan(v.x, v.y), -asin(v.z));"
    "    uv *= invAtan;"
    "    uv += 0.5;"
    "    return uv;"
    "}"
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
    "vec3 Hash(vec3 a)"
    "{"
    "   a = fract(a*0.8);"
    "   a += dot(a, a.xyz + 19.19);"
    "   return fract((a.xxy + a .yxx) * a.zyx);"
    "}"
    "float GetDepth(vec2 p)"
    "{"
    "       vec3 depthPixel   = texture2D(map_depth, p*view_ratio).rgb;" \
    "       float depth       = ((depthPixel.b*"<<1.0/255.0<<"+depthPixel.g)*"<<1.0/255.0<<"+depthPixel.r);"
    "       return "<<0.5*PBRTextureAsset::DEPTH_BUFFER_NORMALISER_INV<<
    "                          -depth*"<< PBRTextureAsset::DEPTH_BUFFER_NORMALISER_INV <<";"
    "}"
    "vec2 RaySearch(vec3 start, vec3 end, int under)"
    "{"
    "   vec3 cur = start;"
    "   vec3 diff = end-start;"
    "   float step = 0.5;"
    "   for(int i = 0 ; i < 5 ; ++i){"
    "       vec3 test = cur + diff*step;"
    "       if((1-2*under)*GetDepth(test.xy) < (1-2*under)*test.z) {"
    "           cur = test;" //Collision in second segment
    "       }"
    "       step = step*0.5;"
    "   }"
    "   return cur.xy;"
    "}"
    "vec3 RayTrace(vec3 p, vec3 v, float jitter)"
    "{"
    "   const vec3 constantList = vec3(1.0, 0.0, -1.0);"
    "   vec2 oldScreenPos = gl_FragCoord.xy;"
    "   vec2 curScreenPos;"
    "   vec3 oldWorldPos = p;"
    "   vec3 curWorldPos;"
    "   vec3 r = vec3(-1.0);"
    "   int oldUnder = 0;"
    "   int under;"
    //"   return oldScreenPos + 300*(v*p_isoToCartMat).xy * constantList.xz; "
    "   for(int i = 0 ; i < 24 ; ++i) {"
    "       curScreenPos = oldScreenPos + 15*(v*p_isoToCartMat).xy * constantList.xz;"
    "       curWorldPos  = oldWorldPos + 15*v;"
    "       float heightPixel = GetDepth(curScreenPos);"
    "       if(heightPixel - curWorldPos.z > 0) under = 1;"
    "       else under = 0;"
    "       if(under != oldUnder && abs(heightPixel - curWorldPos.z) < 30){"
    "           curScreenPos = RaySearch(vec3(oldScreenPos, oldWorldPos.z), vec3(curScreenPos, curWorldPos.z),oldUnder);"
    "           r.xy = curScreenPos;"
    "           r.z = .2;"
    "           if(abs(GetDepth(curScreenPos+vec2(jitter*i,0)) - curWorldPos.z) < 20)"
    "               r.z += 0.2;"
    "           if(abs(GetDepth(curScreenPos+vec2(-jitter*i,0)) - curWorldPos.z) < 20)"
    "               r.z += 0.2;"
    "           if(abs(GetDepth(curScreenPos+vec2(0,jitter*i)) - curWorldPos.z) < 20)"
    "               r.z += 0.2;"
    "           if(abs(GetDepth(curScreenPos+vec2(0,-jitter*i)) - curWorldPos.z) < 20)"
    "               r.z += 0.2;"
    "           r.z *= min(1.0/(length(curWorldPos-p)*.01), 1.0);"
    "           return r;"
    "       }"
    "       oldScreenPos = curScreenPos;"
    "       oldWorldPos = curWorldPos;"
    "       oldUnder = under;"
    "   }"
    "   return r;"
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
   // "   vec3 rVec = normalize(2*texture2D(map_noise, VaryingTexCoord0/view_ratio/4.0).rgb-1.0);"
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
    "   vec3 rVec = Hash(fragPos);"
    "   vec2 rVec2 = rVec.xy;"
    //"   rVec = normalize(rVec);"
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
   /* "   vec3 orthoViewDirection = vec3("<<cos(m_viewAngle.xyAngle*PI/180)*cos(m_viewAngle.zAngle*PI/180)<<","
                                    <<sin(m_viewAngle.xyAngle*PI/180)*cos(m_viewAngle.zAngle*PI/180)<<","
                                    <<sin(m_viewAngle.zAngle*PI/180)<<");"*/
    "   vec3 reflectionView = reflect(-viewDirection, direction);"
    "   reflectionView += mix(vec3(0.0),rVec,materialPixel.r*.25);"
    "   vec2 uv = SampleSphericalMap(normalize(reflectionView));"
    "   vec3 reflectionColor = texture2DLod(map_environmental, uv, materialPixel.r*8.0).rgb;"
    //"   gl_FragData[1] = vec4(0.5+normalize(reflectionView)*0.5,1.0);"
   // "   reflectionColor = ambientLighting;"
    "   if(enable_SSR == true) {"
    "       vec3 envPos = RayTrace(fragPos,reflectionView,materialPixel.r*0.1) + vec3(SSR_map_shift,0);"
    "       if(envPos.z != -1.0){"
    "           vec3 SSRColor = texture2DLod(map_SSRenv, envPos.xy*view_ratio,materialPixel.r*8.0).rgb;"
  //  "           vec3 reflectionDirection = normalize(2.0*texture2D(map_normal, envPos.xy*view_ratio).rgb-1.0);"
   // "           groundColor *= max(dot(reflectionDirection, reflectionView), 0.0);"
   // "           SSRColor = vec3(1.0) - exp(-SSRColor * 0.5);"
    "               SSRColor = clamp(SSRColor, 0, 1.5);"
    "           vec2 dcoord = smoothstep(.2,.6,abs(vec2(.5) - envPos.xy*view_ratio));"
    "           envPos.z *= clamp(1.0 - (dcoord.x + dcoord.y), 0.0, 1.0);"
    "           reflectionColor = mix(reflectionColor,SSRColor, envPos.z);"
    "       }"
    "   } /*else if(reflectionView.z < 0){"
	"       vec2 groundPos = gl_FragCoord.xy*view_zoom;"
	"       vec3 v = vec3((heightPixel/reflectionView.z)*reflectionView.xy,0.0);"
	"       groundPos.xy += (heightPixel*p_isoToCartZFactor)*constantList.yx "
	"                                  +(v*p_isoToCartMat).xy * constantList.xz;"
    "       vec3 groundColor = max(constantList.yyy,texture2DLod(map_SSRenv, groundPos*view_ratio,materialPixel.r*3.0).rgb) *50.0/(1.0+length(v)+abs(heightPixel));"
    "       reflectionColor = mix(groundColor, reflectionColor, reflectionView.z+1.0); "
    "   }*/"
    "   vec2 envBRDF  = texture2D(map_brdflut, vec2(max(dot(direction, viewDirection), 0.0), 1.0-materialPixel.r)).rg;"
    "   vec3 specularAmbient = reflectionColor* (FAmbient * envBRDF.x + envBRDF.y);"
    "   gl_FragData[0].rgb = (albedoPixel.rgb * irradianceAmbient + specularAmbient); "
    //"   gl_FragData[0].a += clamp(FAmbient.x * envBRDF.x + envBRDF.y,0.0,.5);"
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
	"       if(attenuation > 0) {"
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
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(2.0,-2.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-2.0,2.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(2.0,2.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-2.0,-2.0)*rVec2.xy))*"<<1.0/6.0<<";"
    "               if(shadowing > .2 && shadowing < .8) {"
    "                   shadowing =  shadowing * 0.5"
    "                            +(GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(4.0,0.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(-4.0,0.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(0.0,4.0)*rVec2.xy)"
    "                            +GetShadowCastValue(curShadowMap,depth,shadowPos+vec2(0.0,-4.0)*rVec2.xy))*"<<1.0/8.0<<"; "
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
	"       }"
	""
	"   }"
	"   float brightness = dot(gl_FragData[0].rgb, vec3(0.2126, 0.7152, 0.0722))*albedoPixel.a;"
	"   if(enable_bloom == true && brightness > 1.5)"
	"       gl_FragData[1] = gl_FragData[0];"
	"   else"
    "       gl_FragData[1] = vec4(0.0,0.0,0.0,albedoPixel.a);"
   // "   gl_FragData[0].rgb = 1.0-rVec.xyz*0.5;"
	"} else {"
    "   discard;}"
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
    "       gl_FragColor.rgb += texture2D(bloom_map, gl_TexCoord[0].xy).rgb*0.05;"
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
   // "uniform sampler2D map_material;"
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
    "   vec3 rVec = vec3(2.0*texture2D(map_noise, gl_TexCoord[0].xy/view_ratio/4.0).rg-1.0, 1.0);"
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 rayShift = (rot * p_samplesHemisphere[i]) ;"
	"       vec2 screenShift = (view_zoom* 15.0)*(rayShift*p_isoToCartMat).xy;"
	"       rayShift.z *= "<<15.0*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<";"
	//"       screenShift.y *= -1;"
	"       vec2 screenPos = gl_FragCoord.xy  + screenShift * constantList.xz;"
/*	"       float translucency = texture2D(map_depth, screenPos*view_ratio).b;"
	"       if(translucency != 1.0){"*/
	"       vec3 occl_depthPixel = texture2D(map_depth, screenPos*view_ratio).rgb;"
	"       float occl_height = (occl_depthPixel.b*"<<1.0/255.0<<"+occl_depthPixel.g)*"<<1.0/255.0<<"+occl_depthPixel.r;"
    "       if(occl_height < (heightPixel-rayShift.z) - "<<0.1*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<"  "
    "        && occl_height - (heightPixel-rayShift.z) > "<<-20*PBRTextureAsset::DEPTH_BUFFER_NORMALISER<<")"
    "           --occlusion;"
  //  "       }"
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
    "    gl_FragColor =  gl_Color * "
    "                   (texture2D(texture, gl_TexCoord[0].xy + offset * 1.0)	* 0.06 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.75)	* 0.09 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.5)	* 0.12 + "
    "                    texture2D(texture, gl_TexCoord[0].xy + offset * 0.25)	* 0.15 + "
    "                    texture2D(texture, gl_TexCoord[0].xy)	* 0.16 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 1.0) 	* 0.06 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.75)	* 0.09 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.5)	* 0.12 + "
    "                    texture2D(texture, gl_TexCoord[0].xy - offset * 0.25)	* 0.15 ); "
  /* "    gl_FragColor =  gl_Color * "
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
	"    			texture2D(texture, gl_TexCoord[0].xy - offset * 0.2)	* 0.24173); "*/
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


/** BRDFLUT GENERATING taken from https://learnopengl.com **/
void PBRIsoScene::GenerateBrdflut()
{
    sf::Shader brdflut_shader;

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
    "varying vec2 VaryingTexCoord0; "
    "float VanDerCorpus(uint n, uint base)"
    "{"
    "    float invBase = 1.0 / float(base);"
    "    float denom   = 1.0;"
    "    float result  = 0.0;"
    "    for(uint i = 0u; i < 32u; ++i)"
    "    {"
    "        if(n > 0u)"
    "        {"
    "            denom   = mod(float(n), 2.0);"
    "            result += denom * invBase;"
    "            invBase = invBase / 2.0;"
    "            n       = uint(float(n) / 2.0);"
    "        }"
    "    }"
    "    return result;"
    "}"
    "vec2 Hammersley(uint i, uint N)"
    "{"
    "        return vec2(float(i)/float(N), VanDerCorpus(i, 2u));"
    "}"
    "vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)"
    "{"
    "    float a = roughness*roughness;"
    "    float phi = 2.0 * "<<PI<<" * Xi.x;"
    "    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));"
    "    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);"
    "    vec3 H;"
    "    H.x = cos(phi) * sinTheta;"
    "    H.y = sin(phi) * sinTheta;"
    "    H.z = cosTheta;"
    "    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);"
    "    vec3 tangent   = normalize(cross(up, N));"
    "    vec3 bitangent = cross(N, tangent);"
    "    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;"
    "    return normalize(sampleVec);"
    "}  "
    "float GeometrySchlickGGX(float NdotV, float roughness)"
    "{"
    "    float a = roughness;"
    "    float k = (a * a) / 2.0;"
    "    float nom   = NdotV;"
    "    float denom = NdotV * (1.0 - k) + k;"
    "    return nom / denom;"
    "}"
    "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)"
    "{"
    "    float NdotV = max(dot(N, V), 0.0);"
    "    float NdotL = max(dot(N, L), 0.0);"
    "    float ggx2 = GeometrySchlickGGX(NdotV, roughness);"
    "    float ggx1 = GeometrySchlickGGX(NdotL, roughness);"
    "    return ggx1 * ggx2;"
    "}"
    "vec2 IntegrateBRDF(float NdotV, float roughness)"
    "{"
    "    vec3 V;"
    "    V.x = sqrt(1.0 - NdotV*NdotV);"
    "    V.y = 0.0;"
    "    V.z = NdotV;"
    "    float A = 0.0;"
    "    float B = 0.0;"
    "    vec3 N = vec3(0.0, 0.0, 1.0);"
    "    const uint SAMPLE_COUNT = 1024u;"
    "    for(uint i = 0u; i < SAMPLE_COUNT; ++i)"
    "    {"
    "        vec2 Xi = Hammersley(i, SAMPLE_COUNT);"
    "        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);"
    "        vec3 L  = normalize(2.0 * dot(V, H) * H - V);"
    "        float NdotL = max(L.z, 0.0);"
    "        float NdotH = max(H.z, 0.0);"
    "        float VdotH = max(dot(V, H), 0.0);"
    "        if(NdotL > 0.0)"
    "        {"
    "            float G = GeometrySmith(N, V, L, roughness);"
    "            float G_Vis = (G * VdotH) / (NdotH * NdotV);"
    "            float Fc = pow(1.0 - VdotH, 5.0);"
    "            A += (1.0 - Fc) * G_Vis;"
    "            B += Fc * G_Vis;"
    "        }"
    "    }"
    "    A /= float(SAMPLE_COUNT);"
    "    B /= float(SAMPLE_COUNT);"
    "    return vec2(A, B);"
    "}"
    "void main()"
    "{"
    "    vec2 integratedBRDF = IntegrateBRDF(VaryingTexCoord0.x, VaryingTexCoord0.y);"
    "    gl_FragColor.rg = integratedBRDF;"
    "    gl_FragColor.ba = vec2(0.0,1.0);"
    "}";

    brdflut_shader.loadFromMemory(vertexShader.str(),fragShader.str());
    m_brdf_lut.create(512,512,true);

    sf::RenderTexture renderer;
    renderer.create(512,512,false,true);
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(512,512));
    rect.setTexture(&m_brdf_lut);
    renderer.draw(rect, &brdflut_shader);
    //renderer.display();

    m_brdf_lut.update(renderer.getTexture());
}


void PBRIsoScene::CompileWaterGeometryShader()
{
    std::ostringstream fragShader, vertexShader;

    vertexShader<<
   // "uniform vec2 texture_size;"
    "varying vec2 Coord0; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    //"    VaryingTexCoord0 = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy/texture_size; "
    "    Coord0 = 0.5+0.5*(gl_ModelViewProjectionMatrix * gl_Vertex); "\
    "    gl_FrontColor = gl_Color; "\
    "}";

    fragShader<<
    "uniform float p_height;" \
    "uniform float p_wave_pos;" \
    "uniform float p_wave_amplitude;" \
    "uniform float p_wave_frequency;" \
    "uniform float p_wave_turbulence;" \
    "uniform float p_turbulences_amplitude;" \
    "uniform mat3 p_rotation;"
    "uniform vec2 random_gradients["<<16*16<<"];"
    ""
    "varying vec2 Coord0; "\
    "const vec3 constantList = vec3(1.0, 0.0, -1.0);"
    ""
    "int GridPos(int x, int y)"
    "{"
    "   x = mod(x,16);"
    "   y = mod(y,16);"
    "   return x + y * 16;"
    "}"
    ""
    "float Quintic(float t)"
    "{"
    "   return t*t*t*(t*(6.0*t-15.0)+10.0);"
    "}"
    ""
    "float DQuintic(float t)"
    "{"
    "   return 30.0*t*t*(t*(t-2.0)+1.0);"
    "}"
    ""
    "vec4 PerlinNoise(vec2 pos)"
    "{"
    "   pos = pos * 16;"
    "   vec4 r=(0.0);"
    "   vec2 ULcorner = floor(pos);"
    "   pos = pos - ULcorner;"
    "   vec2 hermitePos = vec2(Quintic(pos.x),Quintic(pos.y));"
    ""
    "   vec2 rg00 = (vec3(random_gradients[GridPos(ULcorner.x,ULcorner.y)],0)*p_rotation).xy;"
    "   vec2 rg10 = (vec3(random_gradients[GridPos(ULcorner.x+1,ULcorner.y)],0)*p_rotation).xy;"
    "   vec2 rg11 = (vec3(random_gradients[GridPos(ULcorner.x+1,ULcorner.y+1)],0)*p_rotation).xy;"
    "   vec2 rg01 = (vec3(random_gradients[GridPos(ULcorner.x,ULcorner.y+1)],0)*p_rotation).xy;"
    ""
    "   float dot00 = dot(rg00,pos);"
    "   float dot10 = dot(rg10,pos-(constantList.xy));"
    "   float dot11 = dot(rg11,pos-(constantList.xx));"
    "   float dot01 = dot(rg01,pos-(constantList.yx));"
    "   "
    ""
    "   float m0010 = mix(dot00,dot10,hermitePos.x);"
    "   float m0111 = mix(dot01,dot11,hermitePos.x);"
    "   r.x = mix(m0010, m0111,hermitePos.y);"
    ""
    "   float dx = (1.0-hermitePos.y)*(hermitePos.x*(rg10.x-rg00.x)+DQuintic(pos.x)*(dot10-dot00)+rg00.x)"
    "             + hermitePos.y*(hermitePos.x*(rg11.x-rg01.x)+DQuintic(pos.x)*(dot11-dot01)+rg01.x);"
    "   float dy = (1.0-hermitePos.y)*(hermitePos.x*(rg10.y-rg00.y)+rg00.y)"
    "           +  hermitePos.y *(hermitePos.x*(rg11.y-rg01.y)+rg01.y)"
    "           + (m0111 - m0010)*DQuintic(pos.y);"
    ""
    "   r.yzw = normalize(cross(vec3(1.0,0.0,dx ), "
    "                            vec3(0.0,1.0,dy)));"
    ""
    "   return r;"
    "}"
    ""
    "vec4 FractalNoise(vec2 pos)"
    "{"
    "   float frequency = .5;"
    "   float amplitude = 1.0;"
    "   float frequencyMult = 4;"
    "   float amplitudeMult = 0.5;"
    "   vec4 r = vec4(0.0);"
    "   for(int i = 0 ; i < 5 ; ++i)"
    "   {"
    "       r += PerlinNoise(pos*frequency) * amplitude;"
    "       amplitude *= amplitudeMult;"
    "       frequency *= frequencyMult;"
    "   }"
    "   return r;"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec2 normalizedCoord = Coord0 - floor(Coord0);"
   // "   vec3 noise = PerlinNoise(normalizedCoord);"
    "   vec4 noise = FractalNoise(normalizedCoord);"
    "   vec4 wave = vec4(0.0);"
    "   wave.x = sin((Coord0.x+p_wave_pos+noise.x*p_wave_turbulence)*p_wave_frequency*"<<PI<<"*2);"
    "   wave.y = -(noise.y * p_wave_turbulence + 1/16.0)* p_wave_frequency*"<<PI<<"*2*cos((Coord0.x+p_wave_pos+noise.x*p_wave_turbulence)*p_wave_frequency*"<<PI<<"*2);"
    "   wave.z = -noise.z * p_wave_turbulence *"<<PI<<"*2*cos((Coord0.x+p_wave_pos+noise.x*p_wave_turbulence)*p_wave_frequency*"<<PI<<"*2);"
    "   wave.w = 1.0;"
    "   wave.yzw = normalize(wave.yzw);"
    "   vec4 water = wave * p_wave_amplitude + noise * p_turbulences_amplitude;"
    "   water = 0.5 + water * 0.5;"
   /* "   vec3 n = normalize(cross(vec3(1.0,0.0,noise.x * 0.25), "
    "                            vec3(0.0,1.0,noise.y * 0.25)));"
    "   n = 0.5+n*0.5;"*/
    //"   gl_FragData["<<PBRAlbedoScreen<<"] = vec4(n.x,n.y,0.5+noise.z*0.5,.8); "
    //"   gl_FragData["<<PBRAlbedoScreen<<"] = vec4(.2,.3,.3,.8); "
   // "   gl_FragData["<<PBRAlbedoScreen<<"] = vec4(.2,.3,.3,.8); "
    "   gl_FragData["<<PBRNormalScreen<<"] = vec4(water.yzw,1.0);"
    "   gl_FragData["<<PBRDepthScreen<<"] = vec4(vec3(water.x),1.0);"
    //"   gl_FragData["<<PBRMaterialScreen<<"] = vec4(.1, 0.0, .5, 1.0); "
   // "   gl_FragData["<<PBRMaterialScreen<<"] = vec4(1, 0, .5, 1.0); "
   "     float foam =  smoothstep(.5,1,water.x);" //NEED TO USE A TEXTURE HERE
   //"     foam = clamp(foam+smoothstep(.5,1.0,noise.w),0.0,0.2);"
    "    gl_FragData["<<PBRAlbedoScreen<<"] = mix(vec4(.1,.15,.15,.95), vec4(.8,.8,.8,.95),foam);"
    "    gl_FragData["<<PBRMaterialScreen<<"] = mix(vec4(.1, 0.0,.9,1.0), vec4(.5, 0.0,.1,1.0),foam);"
    "}";

    m_waterGeometryShader.loadFromMemory(vertexShader.str(), fragShader.str());
}

}


