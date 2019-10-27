#version 420 core

out vec4 FragColor;

layout (std140, binding = 8) uniform EditorData{
	uniform vec4 color;
};

void main(){
	FragColor = color;
}
