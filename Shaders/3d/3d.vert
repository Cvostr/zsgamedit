#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in ivec4 BoneIDs1;

layout (location = 7) in vec4 Weights;
layout (location = 8) in vec4 Weights1;

layout (location = 9) in int bones;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 InNormal;
layout(location = 2) out vec2 UVCoord;
layout(location = 3) out mat3 TBN;

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
	UVCoord = uv;
	InNormal = normal;
		
	mat4 bone_t = mat4(1.0);

	if(bones > 0)
		bone_t = getBoneTransform();

	vec4 vertpos = object_transform * bone_t * vec4(position, 1.0);

	FragPos = (vertpos).xyz;
	
	vec3 TangentVec = normalize(vec3(object_transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(object_transform * vec4(bitangent, 0)));
	vec3 NormalVec = normalize(vec3(object_transform * vec4(normal, 0)));
	TBN = transpose(mat3(TangentVec, BiTangentVec, NormalVec));
		
	gl_Position = cam_projection * cam_view * vertpos;
	
}
