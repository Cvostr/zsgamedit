#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

out vec4 FragColor;

uniform vec4 color;

void main(){
	FragColor = color;
}
