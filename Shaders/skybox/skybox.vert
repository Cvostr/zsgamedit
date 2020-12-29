#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 6) uniform Skybox{
	uniform mat4 projection;
	uniform mat4 view;
};

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos * 2, 1.0);
}  
