#version 420 core

#define RENDER_SPRITE 1
#define RENDER_TEXT 2

layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;

layout (std140, binding = 7) uniform Ui{
    mat4 cam_projection;
    mat4 object_transform;
	int render_mode;
    vec3 text_color;
};

layout(binding = 0) uniform sampler2D sprite_map;

void main(){

vec4 color;

if(render_mode == RENDER_SPRITE)
	color = texture(sprite_map, _UV);

if(render_mode == RENDER_TEXT)
	color = texture(sprite_map, _UV).r * vec4(text_color, 1.0);

FragColor = color;

}
