#version 430 core


layout(location = 0) in vec2 _UV;
layout(location = 0) out vec4 FragColor;

layout (std140, binding = 7) uniform UiSprite{
    mat4 cam_projection_ui;
    mat4 object_transform_ui;
    vec3 ui_color;
};

layout(binding = 0) uniform sampler2D sprite_map;

void main(){
	vec4 color = texture(sprite_map, _UV) * vec4(ui_color, 1.0);

    FragColor = color;
}
