#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

#define RENDER_SPRITE 1
#define RENDER_TEXT 2

in vec2 _UV;
out vec4 FragColor;

uniform vec3 text_color;

uniform int render_mode;

uniform sampler2D sprite_map;

void main(){

vec4 color;

if(render_mode == RENDER_SPRITE)
	color = texture(sprite_map, _UV);

if(render_mode == RENDER_TEXT)
	color = texture(sprite_map, _UV).r * vec4(text_color, 1.0);

FragColor = color;

}
