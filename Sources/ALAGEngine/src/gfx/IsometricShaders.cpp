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
    "uniform sampler2D depthMap;" \
    "uniform sampler2D normalMap;" \
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
    "   float heightPixel = depthPixel.b*height;"
	"   direction = direction * normalProjMat;"
	"   vertex.y -= heightPixel*isoToCartZFactor;"
	"   vertex = vertex*cartToIso2DProjMat;"
	"   vertex.z = heightPixel + zPos;"
    "   gl_FragDepth = 1.0 - depthPixel.a*(0.5+vertex.z*0.0001);" \
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
	"		vec3 light_direction = gl_LightSource[i].position.xyz - vertex.xyz;" \
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


const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 colorPixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   vec4 depthPixel = texture2D(depthMap, gl_TexCoord[0].xy);" \
    "   float zPixel = depthPixel.b*height + zPos;" \
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


