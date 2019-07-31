#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 cam_projection;
uniform mat4 cam_view;
uniform mat4 object_transform;

void main(){
	
	gl_Position = cam_projection * cam_view * object_transform * vec4(position, 1.0);
	
}
