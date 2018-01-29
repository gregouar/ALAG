#include <iostream>

const std::string vertexShader = \
    "varying vec3 vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";


const std::string depthAndLighting_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depthMap;" \
    "uniform bool useNormalMap;" \
    "uniform sampler2D normalMap;" \
    "uniform bool useSSAO;" \
    "uniform sampler2D geometryMap;"
    "uniform vec2 screen_ratio;" \
    "uniform mat3 normalProjMat;" \
    "uniform mat3 cartToIso2DProjMat;" \
    "uniform float isoToCartZFactor;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
	"	vec3 direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
    "   float heightPixel = 0; "
    "   if(useDepthMap == true)"
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height;"
	"   direction = direction * normalProjMat;"
	"   vec3 frag_pos = vertex;"
	"   frag_pos.y -= heightPixel*isoToCartZFactor;"
	"   frag_pos = frag_pos*cartToIso2DProjMat;"
	"   frag_pos.z = heightPixel + zPos;"\
    /*"   gl_FragDepth = 1.0 - colorPixel.a*(0.5+vertex.z*0.0001);" \*/
	"   float occlusion =  1.0- (texture2D(geometryMap, (gl_FragCoord+vec2(0,0))*screen_ratio).b*4"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(1,0))*screen_ratio).b*2"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(-1,0))*screen_ratio).b*2"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(0,1))*screen_ratio).b*2"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(0,-1))*screen_ratio).b*2"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(1,1))*screen_ratio).b*1"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(1,-1))*screen_ratio).b*1"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(-1,-1))*screen_ratio).b*1"
	"                      +texture2D(geometryMap, (gl_FragCoord+vec2(1,-1))*screen_ratio).b*1"
	"                       )/16.0;"
	"/*occlusion =  1.0- texture2D(geometryMap, (gl_FragCoord+vec2(0,0))*screen_ratio).b;*/"
	"   "
    "  if(colorPixel.a > 0.2){"
    "   gl_FragDepth = 1.0 - colorPixel.a*(0.5+frag_pos.z*0.001);} else {gl_FragDepth = 1.0;}" \
    "   gl_FragColor = gl_Color *ambient_light * colorPixel;" \
    "int i;" \
	"for(i = 0 ; i < NBR_LIGHTS ; i = i+1)" \
	"{" \
	"	float lighting = 0.0;" \
	"	if(gl_LightSource[i].position.w == 0.0)" \
	"	{		" \
	"		vec3 light_direction = -gl_LightSource[i].position.xyz;" \
    "       lighting = 1.0/( gl_LightSource[i].constantAttenuation);"
	"       if(useNormalMap == true)"
	"		    lighting *= max(0.0, dot(direction,normalize(light_direction)));"
	"	}" \
	"	else" \
	"	{" \
	"		vec3 light_direction = gl_LightSource[i].position.xyz - frag_pos.xyz;" \
	"		float dist = length(light_direction);" \
	"		lighting = 1.0/( gl_LightSource[i].constantAttenuation +" \
	"								  dist*gl_LightSource[i].linearAttenuation +" \
	"								  dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"       if(useNormalMap == true){"
	"	    	lighting *= max(0.0, dot(direction,normalize(light_direction)));}" \
	"	}" \
	"	lighting *= gl_LightSource[i].diffuse.a;" \
	"	gl_FragColor.rgb +=  gl_Color.rgb * colorPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;" \
	"}" \
    "   gl_FragColor.rgb *= occlusion;"
    "}";


const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   float zPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height + zPos;" \
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+zPixel*0.0001);" \
    "   gl_FragColor = gl_Color * colorPixel; " \
    "}";


const std::string lighting_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D normalMap;" \
    "uniform float zPos;" \
    "uniform float isoToCartZFactor;" \
    "uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   gl_FragColor = gl_Color * colorPixel * ambient_light; " \
    "}";


const std::string geometry_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depthMap;" \
    "uniform bool useNormalMap;" \
    "uniform sampler2D normalMap;" \
    "uniform sampler2D geometryMap;"
    "uniform vec2 screen_ratio;" \
    "uniform mat3 normalProjMat;" \
    "uniform mat3 normalProjMatInv;" \
    "uniform mat3 cartToIso2DProjMat;" \
    "uniform mat3 isoToCartMat;" \
    "uniform float isoToCartZFactor;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
	"	vec3 direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
    "   float heightPixel = 0; "
    "   if(useDepthMap == true)"
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height;"
	"   direction = direction * normalProjMat;"
	"   vec3 frag_pos = vertex;"
	"   frag_pos.y -= heightPixel*isoToCartZFactor;"
	"   frag_pos = frag_pos*cartToIso2DProjMat;"
	"   frag_pos.z = heightPixel + zPos;"\
    "  if(colorPixel.a > 0.2){"
    "   gl_FragDepth = 1.0 - colorPixel.a*(0.5+frag_pos.z*0.001);} else {gl_FragDepth = 1.0;}" \
    "   gl_FragColor.r = gl_FragDepth;" \
    "   gl_FragColor.g = (gl_FragDepth - 256.0 * floor(gl_FragDepth / 256.0))*256.0;" \
    "   float occlusion = 0;"
	"   vec3 t = normalize(vec3(1,1,0) - direction * dot(vec3(1,1,0), direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int x = -3 ; x <= 3 ; ++x){"
	"   for(int y = -3 ; y <= 3 ; ++y){"
	"       vec3 decal = rot * normalize(vec3(x, y, 1)) *15;"
	"       vec3 screen_pos = gl_FragCoord  + decal*isoToCartMat;"
	"       float occl_height = texture2D(geometryMap, (screen_pos.xy)*screen_ratio).r"
    "                          +texture2D(geometryMap, (screen_pos.xy)*screen_ratio).g/256.0;"
    "       occl_height = (0.5-occl_height)*1000;"
    "       if(occl_height > (frag_pos.z+decal.z) + 2  "
    "        && occl_height - (frag_pos.z+decal.z) < 20)"
    "           occlusion += 1.0;"
    "          "
	"   }} "
    "   gl_FragColor.b = occlusion/25.0;" \
    "   gl_FragColor.a = colorPixel.a;" \
    "}";



