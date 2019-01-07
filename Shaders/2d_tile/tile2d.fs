#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec3 tDiffuse;
layout (location = 2) out vec3 tPos;

out vec4 FragColor;

in vec3 FragPos;
in vec2 UVCoord;

//textures
uniform sampler2D diffuse;

uniform bool hasDiffuseMap;

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 0.078, 0.574); //Default value
	if(hasDiffuseMap)
		result = texture(diffuse, uv).xyz ;
		
	FragColor = vec4(result, 1);
	tDiffuse = result;
	tPos = FragPos;
}
