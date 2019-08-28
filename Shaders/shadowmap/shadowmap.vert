#version 420 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;


layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform mat4 LightProjectionMat; // 16 * 4
    uniform mat4 LightViewMat; 
     // 16 * 4
    uniform float shadow_bias; //4
    uniform bool hasShadowMap; //4
};

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

void main(){
	
	gl_Position = LightProjectionMat * LightViewMat * object_transform * vec4(position, 1.0);
	
}
