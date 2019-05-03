#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;

out vec4 FragColor;

uniform vec3 color;

void main(){
	
	FragColor = vec4(color, 1);
	tDiffuse = FragColor;
}
