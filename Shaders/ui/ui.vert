#version 420 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec2 _UV;

layout (std140, binding = 7) uniform UiSprite{
    mat4 cam_projection_ui;
    mat4 object_transform_ui;
    vec3 ui_color;
};

void main(){
    _UV = uv;
    gl_Position = cam_projection_ui * object_transform_ui * vec4(pos, 1);
}
