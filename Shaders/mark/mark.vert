#version 420 core

layout (location = 0) in vec3 position;


layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};
void main(){

	gl_Position =  cam_projection * cam_view * object_transform * vec4(position, 1.0);
	
}
