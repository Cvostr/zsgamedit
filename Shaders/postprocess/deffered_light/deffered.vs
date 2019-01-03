#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec3 FragPos;
out vec2 UVCoord;

void main(){
	UVCoord = uv;
	
	//FragPos = (object_transform * vec4(position, 1.0)).xyz;

	gl_Position = vec4(position, 1.0);
	
}
