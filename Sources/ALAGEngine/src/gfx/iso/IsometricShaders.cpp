#include <iostream>

const std::string vertexShader = \
    "varying vec3 v_vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    v_vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";


const std::string color_fragShader = \
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
    "}";

const std::string depth_fragShader = \
    "uniform sampler2D map_color;" \
    "uniform bool enable_depthMap;" \
    "uniform sampler2D map_depth;" \
    "uniform float p_height;" \
    "uniform float p_zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(map_color, gl_TexCoord[0].xy).a;" \
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
    "}";

const std::string normal_fragShader = \
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
	"	vec3 direction = vec3(0,0,1);"
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
    "}";

    /** REPLACE 8 SHADOW MAP BY CONSTANT**/
const std::string lighting_fragShader = \
    "uniform sampler2D map_color;" \
    "uniform sampler2D map_normal;" \
    "uniform sampler2D map_depth;" \
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
    "uniform bool enable_SSAO;" \
    "uniform sampler2D map_SSAO;" \
    "varying vec3 v_vertex; "\
    ""
    "float GetShadowCastValue(int curShadowMap, float heightPixel, vec2 shadowPos) {"
    "   vec4 shadowPixel;"
    "   vec2 mapPos = (shadowPos-shadow_shift[curShadowMap])*shadow_ratio[curShadowMap];"
    "   if(mapPos.x > 1.0) mapPos.x = 1.0;"
    "   if(mapPos.y > 1.0) mapPos.y = 1.0;"
    "   if(mapPos.x < 0) mapPos.x = 0;"
    "   if(mapPos.y < 0)  return 0;"
	"   if(curShadowMap == 0)"
	"       shadowPixel = texture2D(shadow_map_0, mapPos);"
	"   float shadowHeight = (0.5-(shadowPixel.r+shadowPixel.g/256.0+shadowPixel.b/65536.0))*1000;"
    "   return 1.0 - min(1.0,max(0.0, (shadowHeight-heightPixel-5)*0.05));"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 colorPixel  = texture2D(map_color, gl_TexCoord[0].xy);" \
    "   vec4 normalPixel = texture2D(map_normal, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel  = texture2D(map_depth, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*normalPixel.rgb;"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*1000;"
	"   vec3 fragPos = v_vertex*view_zoom+vec3(view_shift.xy,0);"
	"   fragPos.y -= heightPixel*p_isoToCartZFactor;"
	"   fragPos = fragPos*p_cartToIso2DProjMat;"
	"   fragPos.z = heightPixel;"
    "   gl_FragColor = gl_Color * light_ambient * colorPixel; "
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
	"       if(curShadowMap < 8 && shadow_caster[curShadowMap] == i) {"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec2 shadowPos = gl_FragCoord.xy;"
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
	"	    gl_FragColor.rgb +=  gl_Color.rgb * colorPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;"
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
    "}";


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




