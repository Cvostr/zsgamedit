#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{   
	tDiffuse.xyz = texture(skybox, TexCoords).xyz;
}
