#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 UVCoord;

layout(binding = 0) uniform TransformUniform {
    mat4 object_transform;
    mat4 cam_view;
    mat4 cam_projection;
} Transform;

void main(){
	UVCoord = uv;
	
	FragPos = (Transform.object_transform * vec4(position, 1.0)).xyz;

	gl_Position = Transform.cam_projection * Transform.cam_view * vec4(FragPos, 1.0);
	
}
