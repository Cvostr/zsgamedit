#version 420 core

layout (location = 0) in vec3 position;

layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs1;

layout (location = 7) in vec4 Weights;
layout (location = 8) in vec4 Weights1;

layout (location = 9) in int bones;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (std140, binding = 4) uniform BonesData{
    uniform mat4 bone_transform[150];
};

mat4 getBoneTransform(){
    int _ids[8];
    float _weights[8];
    
    _ids[0] = BoneIDs.x;
    _ids[1] = BoneIDs.y;
    _ids[2] = BoneIDs.z;
    _ids[3] = BoneIDs.w;
    _ids[4] = BoneIDs1.x;
    _ids[5] = BoneIDs1.y;
    _ids[6] = BoneIDs1.z;
    _ids[7] = BoneIDs1.w;
    
    _weights[0] = Weights.x;
    _weights[1] = Weights.y;
    _weights[2] = Weights.z;
    _weights[3] = Weights.w;
    _weights[4] = Weights1.x;
    _weights[5] = Weights1.y;
    _weights[6] = Weights1.z;
    _weights[7] = Weights1.w;
    
    mat4 result = mat4(0.0);
    
    for(int i = 0; i < 8; i++){
        result += bone_transform[_ids[i]] * _weights[i];
   }
	
    return result;
}

void main(){

	mat4 bone_t = mat4(1.0);

	if(bones > 0)
		bone_t = getBoneTransform();

	gl_Position =  cam_projection * cam_view * object_transform * bone_t * vec4(position, 1.0);
	
}
