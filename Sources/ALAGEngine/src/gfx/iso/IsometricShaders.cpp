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


const std::string color_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   float heightPixel = 0; "
    "   if(useDepthMap == true){"
    "        vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height;"
    "   }"
    "   float zPixel = heightPixel + zPos;" \
    "   gl_FragDepth = 1.0 - colorPixel.a*(0.5+zPixel*0.001);" \
    "   gl_FragColor = gl_Color * colorPixel; " \
    "}";

const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(colorMap, gl_TexCoord[0].xy).a;" \
    "   float heightPixel = 0; "
    "   if(useDepthMap == true){"
    "        vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height;"
    "   }"
    "   float zPixel = heightPixel + zPos;" \
    "   gl_FragDepth = 1.0 - colorAlpha*(0.5+zPixel*0.001);" \
    "   gl_FragColor.r = gl_FragDepth;" \
    "   gl_FragColor.g = (gl_FragDepth - 256.0 * floor(gl_FragDepth / 256.0))*256.0;" \
    "   gl_FragColor.b = (gl_FragDepth - 65536.0 * floor(gl_FragDepth / (65536.0)))*65536.0;" \
    "   gl_FragColor.a = colorAlpha;" \
    "}";

const std::string normal_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depthMap;" \
    "uniform bool useNormalMap;" \
    "uniform sampler2D normalMap;" \
    "uniform mat3 normalProjMat;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorAlpha = texture2D(colorMap, gl_TexCoord[0].xy).a;" \
	"	vec3 direction = vec3(0,0,1);"
	"   if(useNormalMap == true){"
	"   direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
	"   }"
    "   float heightPixel = 0; "
    "   if(useDepthMap == true){"
    "        vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "       heightPixel = (depthPixel.r+depthPixel.g+depthPixel.b)*.33*height;"
    "   }"
    "   float zPixel = heightPixel + zPos;" \
    "   gl_FragDepth = 1.0 - colorAlpha*(0.5+zPixel*0.001);" \
	"   direction = direction * normalProjMat;"
    "   gl_FragColor.rgb = 0.5+direction*0.5;" \
    "   gl_FragColor.a = colorAlpha;" \
    "}";

const std::string lighting_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D normalMap;" \
    "uniform sampler2D depthMap;" \
    "uniform bool useSSAO;" \
    "uniform sampler2D SSAOMap;" \
    "uniform float zPos;" \
    "uniform mat3 cartToIso2DProjMat;" \
    "uniform float isoToCartZFactor;" \
    "uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "uniform vec2 screen_ratio;" \
    "uniform float zoom;"
    "uniform vec2 view_decal;" \
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 normalPixel = texture2D(normalMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*1000;"
	"   vec3 frag_pos = vertex*zoom+vec3(view_decal.xy,0);"
	"   frag_pos.y -= heightPixel*isoToCartZFactor;"
	"   frag_pos = frag_pos*cartToIso2DProjMat;"
	"   frag_pos.z = heightPixel;"
    "   gl_FragColor = gl_Color * ambient_light * colorPixel; "
    "   for(int i = 0 ; i < NBR_LIGHTS ; ++i)" \
	"   {" \
	"	    float lighting = 0.0;" \
	"       vec3 light_direction = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	light_direction = -gl_LightSource[i].position.xyz;" \
    "           lighting = 1.0/( gl_LightSource[i].constantAttenuation);"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	light_direction = gl_LightSource[i].position.xyz - frag_pos.xyz;" \
	"	    	float dist = length(light_direction);" \
	"	    	lighting = 1.0/( gl_LightSource[i].constantAttenuation +" \
	"	    							  dist*gl_LightSource[i].linearAttenuation +" \
	"	    							  dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"	    }" \
	"       lighting *= max(0.0, dot(direction,normalize(light_direction)));"
	"	    lighting *= gl_LightSource[i].diffuse.a;" \
	"	    gl_FragColor.rgb +=  gl_Color.rgb * colorPixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;" \
	"   }"
    "   if(useSSAO == true) {"
	"       float occlusion  = (texture2D(SSAOMap, gl_TexCoord[0].xy+vec2(0,0)*screen_ratio).b*4"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(1,0))*screen_ratio).b*2"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(-1,0))*screen_ratio).b*2"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(0,2))*screen_ratio).b*2"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(0,-2))*screen_ratio).b*2"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(1,2))*screen_ratio).b*1"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(1,-2))*screen_ratio).b*1"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(-1,-2))*screen_ratio).b*1"
	"                      +texture2D(SSAOMap, gl_TexCoord[0].xy+(vec2(1,-2))*screen_ratio).b*1"
	"                       )/16.0;"
    "       gl_FragColor.rgb *= occlusion;"
	"   };"
    "}";


const std::string SSAO_fragShader = \
    "uniform sampler2D normalMap;" \
    "uniform sampler2D depthMap;" \
    "uniform sampler2D noiseMap;" \
    "uniform float zPos;" \
    "uniform mat3 isoToCartMat;" \
    "uniform mat3 cartToIso2DProjMat;" \
    "uniform float isoToCartZFactor;" \
    "uniform vec2 screen_ratio;" \
    "uniform float zoom;"
    "uniform vec3 samplesHemisphere[16];"
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 normalPixel = texture2D(normalMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0+2.0*texture2D(normalMap, gl_TexCoord[0].xy).rgb;"
    "   float heightPixel = (0.5-(depthPixel.r+depthPixel.g/256.0+depthPixel.b/65536.0))*1000;"
	"   vec3 frag_pos = vertex;"
	"   frag_pos.y -= heightPixel*isoToCartZFactor;"
	"   frag_pos = frag_pos*cartToIso2DProjMat;"
	"   frag_pos.z = heightPixel;"
    "   float occlusion = 0;"
    "   vec3 rVec = -1.0+2.0*texture2D(noiseMap, gl_TexCoord[0].xy);"
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));"
	"   mat3 rot = mat3(t,cross(direction,t),direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 decal = rot * samplesHemisphere[i] * 20;"
	"       vec3 screen_pos = gl_FragCoord  + zoom*decal*isoToCartMat;"
	"       vec3 occl_depthPixel = texture2D(depthMap, (screen_pos.xy)*screen_ratio);"
	"       float occl_height = (0.5-(occl_depthPixel.r+occl_depthPixel.g/256.0+occl_depthPixel.b/65536.0))*1000;"
    "       if(occl_height > (frag_pos.z+decal.z) + 1  "
    "        && occl_height - (frag_pos.z+decal.z) < 20)"
    "           occlusion += 1.0;"
	"   } "
    "   gl_FragColor.rgb = 1.0-occlusion/12.0;" \
    "   gl_FragColor.a = 1;" \
    "}";




