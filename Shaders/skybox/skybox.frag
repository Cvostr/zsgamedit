#version 420 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 4) out vec4 tMasks;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{   
	tDiffuse.xyz = texture(skybox, TexCoords).xyz;
	tMasks = vec4(0, 0, 0, 0);
}
