#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;

out vec4 FragColor;

uniform int isTransformMark = 0;

void main(){
	
	FragColor = vec4(0.23, 0.23, 0.54, 1);
	if(isTransformMark == 1) FragColor = vec4(1, 1, 0.0, 1);
	tDiffuse = FragColor;
}
