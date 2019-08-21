#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 UVCoord;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    //Camera position
    uniform vec3 cam_position;
};

void main(){
	UVCoord = uv;
	
	FragPos = (object_transform * vec4(position, 1.0)).xyz;

	gl_Position =  cam_projection * cam_view * vec4(FragPos, 1.0);
	
}
