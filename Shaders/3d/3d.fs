#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tMasks;

out vec4 FragColor;

in vec3 FragPos;
in vec3 InNormal;
in vec2 UVCoord;
in mat3 TBN;
//in vec3 ShadowProjection;

//textures
uniform sampler2D diffuse;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D shadow_map;

uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasSpecularMap;
uniform bool hasShadowMap = false;

uniform float material_shininess;

uniform vec3 diffuse_color = vec3(1.0, 0.078, 0.574);

//Shadowmapping stuff
uniform mat4 LightProjectionMat;
uniform mat4 LightViewMat;
uniform float shadow_bias;

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 1.0, 1.0); //Default value
	vec3 Normal = InNormal; //defaultly, use normals from mesh
	float result_shininess = material_shininess;
	
	if(hasDiffuseMap)
		result = texture(diffuse, uv).xyz ;
		
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
            
                vec4 shadowmap = texture(shadow_map, ShadowProjection.xy + _offset / 2048);
                float texture_depth = shadowmap.r;
                tMasks.g += (real_depth - shadow_bias > texture_depth) ? 0.01 : 0.0;
            }
        }
        
        
        if(real_depth > 1.0) tMasks.g = 0.0;
        
	}
}
