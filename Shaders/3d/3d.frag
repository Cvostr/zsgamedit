#version 420 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tMasks;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;

//textures
layout(binding = 0) uniform sampler2D diffuse;
layout(binding = 1) uniform sampler2D normal_map;
layout(binding = 2) uniform sampler2D specular_map;
layout(binding = 3) uniform sampler2D height_map;
layout(binding = 6) uniform sampler2D shadow_map;

layout (std140) uniform Default3d{
    vec3 diffuse_color;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasSpecularMap;
    bool hasHeightMap;

    float material_shininess;
};

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    //Camera position
    uniform vec3 cam_position;
};

layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform mat4 LightProjectionMat; // 16 * 4
    uniform mat4 LightViewMat; // 16 * 4
    uniform float shadow_bias; //4
    uniform bool hasShadowMap; //4
    uniform int shadowmap_Width; //4
    uniform int shadowmap_Height; //4
};

vec2 processParallaxMapUv(vec2 uv){
    if(!hasHeightMap) return uv;
    
    float height = texture(height_map, uv).r;
    vec3 camera_dir = normalize(TBN * FragPos - TBN * cam_position);
    
    vec2 uv_offset = camera_dir.xy / camera_dir.z * (height);
    return uv - uv_offset;
}

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 1.0, 1.0); //Default value
	vec3 Normal = InNormal; //defaultly, use normals from mesh
	float result_shininess = material_shininess;
	
	if(hasDiffuseMap)
		result = texture(diffuse, processParallaxMapUv(uv)).xyz ;
		
	result *= diffuse_color;
	
    if(hasNormalMap){
        Normal = texture(normal_map, uv).xyz;
		Normal = normalize(Normal * 2 - 1);
		Normal = normalize(TBN * Normal);
	}    
		
    if(hasSpecularMap)
        result_shininess *= texture(specular_map, uv).r;
            
	tDiffuse = vec4(result, result_shininess);
	tPos = FragPos;
	tNormal = Normal;

	tMasks = vec4(1.0, 0, 0, 0);
	//Shadowmapping enabled
	if(hasShadowMap){
        vec4 objPosLightSpace = LightProjectionMat * LightViewMat * vec4(FragPos, 1.0);
        vec3 ShadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w) / 2.0 + 0.5;
	
        float real_depth = ShadowProjection.z;

        for(int x = 0; x < 8; x ++){
            for(int y = 0; y < 8; y ++){
                vec2 _offset = vec2(x, y);

                _offset.x /= shadowmap_Width;
                _offset.y /= shadowmap_Height;

                vec4 shadowmap = texture(shadow_map, ShadowProjection.xy + _offset);
                float texture_depth = shadowmap.r;
                tMasks.g += (real_depth - shadow_bias > texture_depth) ? 0.01 : 0.0;
            }
        }
        
        
        if(real_depth > 1.0) tMasks.g = 0.0;
        
	}
}
