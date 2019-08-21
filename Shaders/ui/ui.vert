#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 _UV;

uniform mat4 cam_projection;
uniform mat4 object_transform;

void main(){

_UV = uv;

gl_Position = cam_projection * object_transform * vec4(pos, 1);


}
