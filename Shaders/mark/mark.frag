#version 420 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 4) out vec4 tMasks;

out vec4 FragColor;

layout (std140, binding = 8) uniform EditorData{
	uniform vec3 color;
};

void main(){
	
	FragColor = vec4(color, 1);
	tMasks = vec4(0,0,0,0);
	tDiffuse = FragColor;
}
