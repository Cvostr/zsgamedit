#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}  
