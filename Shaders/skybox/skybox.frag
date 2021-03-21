#version 420 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 4) out vec4 tMasks;

in vec3 TexCoords;

layout(binding = 0) uniform samplerCube skybox;

layout (std140, binding = 3) uniform Particle{
    vec3 tint_color;
	float Exposure;
};

void main()
{   
	vec3 color = texture(skybox, TexCoords).rgb;
	color *= tint_color;
	color *= Exposure;

	tDiffuse.xyz = color;
	tMasks = vec4(0, 0, 0, 0);
}
