#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec3 InNormal;
out vec2 UVCoord;
out mat3 TBN;
//out vec3 ShadowProjection;

uniform mat4 cam_projection;
uniform mat4 cam_view;
uniform mat4 object_transform;


void main(){
	UVCoord = uv;
	InNormal = normal;
	
	FragPos = (object_transform * vec4(position, 1.0)).xyz;
	
	vec3 TangentVec = normalize(vec3(object_transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(object_transform * vec4(bitangent, 0)));
	vec3 NormalVec = normalize(vec3(object_transform * vec4(normal, 0)));
	TBN = transpose(mat3(TangentVec, BiTangentVec, NormalVec));
	
	gl_Position =  cam_projection * cam_view * vec4(FragPos, 1.0);
	
}
