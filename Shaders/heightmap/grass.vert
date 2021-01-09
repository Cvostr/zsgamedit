#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 InNormal;
layout (location = 2) out vec2 UVCoord; 
layout (location = 3) out mat3 TBN;

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
	uniform vec3 cam_position;
};

layout (std140, binding = 9) uniform InstMatrices{
    uniform mat4 inst_transform[1000];
};

layout (std140, binding = 11) uniform Time{
    uniform uint Frames;
	uniform float DeltaTime;
};

vec3 CalcWind(int speed, float amplitude){

	vec3 Pos = position;
	float NormTime = float((Frames) % speed) / speed;
	uint full_cycle = Frames % (speed * 4);
	//Calculate wind
	if(Pos.y == 0.5){
		if(full_cycle <= speed)
			Pos.x += NormTime * amplitude;
		if(full_cycle >= speed && full_cycle < speed * 2)
			Pos.x += amplitude - NormTime * amplitude;
		if(full_cycle >= speed * 2 && full_cycle < speed * 3)
			Pos.x += -NormTime * amplitude;
		if(full_cycle >= speed * 3)
			Pos.x += -amplitude + NormTime * amplitude;
	}
	return Pos;
}

void main(){
	UVCoord = uv;
	InNormal = normal;
	
	vec3 Pos = CalcWind(100, 1.f / 10);

	FragPos = (object_transform * inst_transform[gl_InstanceID] * vec4(Pos, 1.0)).xyz;

	//Calculate TBN
	vec3 TangentVec = normalize(vec3(object_transform * vec4(tangent, 0)));
	vec3 BiTangentVec = normalize(vec3(object_transform * vec4(bitangent, 0)));
	vec3 NormalVec = normalize(vec3(object_transform * vec4(normal, 0)));
	TBN = transpose(mat3(TangentVec, BiTangentVec, NormalVec));
	
	gl_Position =  cam_projection * cam_view * vec4(FragPos, 1.0);
	
}
 
