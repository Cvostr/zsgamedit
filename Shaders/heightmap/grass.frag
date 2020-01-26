#version 420 core

#define TEXTURES_AMOUNT 12

layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tMasks;

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 InNormal;
in vec2 UVCoord;
in mat3 TBN;
in vec3 _id;

//textures
layout(binding = 0) uniform sampler2D diffuse;


layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};


void main(){

	vec2 uv = UVCoord;
	
	vec4 diffuse = texture(diffuse, uv);
	
	if(diffuse.a == 0)
       discard;
        
    FragColor = vec4(diffuse.xyz, 0);    
}
 
