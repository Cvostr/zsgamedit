#version 420 core

#define TEXTURES_AMOUNT 8

layout (location = 0) out vec4 tDiffuse;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tMasks;

out vec4 FragColor;

in vec3 FragPos;
in vec3 InNormal;
in vec2 UVCoord;
in mat3 TBN;
in vec3 _id;

//textures
uniform sampler2D diffuse[TEXTURES_AMOUNT];

uniform sampler2D texture_mask;
uniform sampler2D texture_mask1;
uniform sampler2D texture_mask2;

uniform int isPicking;
uniform bool hasShadowMap = false;

//Shadowmapping stuff
uniform sampler2D shadow_map;
uniform mat4 LightProjectionMat;
uniform mat4 LightViewMat;
uniform float shadow_bias;

float getFactor(int id, vec2 uv){
    vec4 mask;
	int maskid = id / 4;
	
	switch(maskid){
        case 0:{
            mask = texture(texture_mask, uv);
            break;
        }
        case 1:{
            mask = texture(texture_mask1, uv);
            break;
        }
        case 2:{
            mask = texture(texture_mask2, uv);
            break;
        }
	}
    
    float factor = 0;
    int factorid = id % 4;
    
    switch(factorid){
        case 0:{
            factor = mask.r;
            break;
        }
        case 1:{
            factor = mask.g;
            break;
        }
        case 2:{
            factor = mask.b;
            break;
        }
        case 3:{
            factor = mask.a;
            break;
        }
    
    }

    return factor;
}

vec3 getDiffuse(int id, vec2 uv, int multiplyer){
    switch(id){
        case 0:{
            return texture(diffuse[0], uv * multiplyer).xyz;
            break;
        }
        case 1:{
            return texture(diffuse[1], uv * multiplyer).xyz;
            break;
        }
        case 2:{
            return texture(diffuse[2], uv * multiplyer).xyz;
            break;
        }
        case 3:{
            return texture(diffuse[3], uv * multiplyer).xyz;
            break;
        }
        case 4:{
            return texture(diffuse[4], uv * multiplyer).xyz;
            break;
        }
        case 5:{
            return texture(diffuse[5], uv * multiplyer).xyz;
            break;
        }
        case 6:{
            return texture(diffuse[6], uv * multiplyer).xyz;
            break;
        }
        case 7:{
            return texture(diffuse[7], uv * multiplyer).xyz;
            break;
        }
        
    }
}

vec3 getFragment(vec2 uv, int multiplyer){
    vec3 result = vec3(0,0,0);

    for(int i = 0; i < TEXTURES_AMOUNT; i ++){
        float factor = getFactor(i, uv);
        vec3 diffuse = getDiffuse(i, uv, multiplyer);
        
        result = mix(result, diffuse, factor);
    }
        
    return result;
}

void _shadow(){
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

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 1.0, 1.0); //Default value
	vec3 Normal = InNormal; //defaultly, use normals from mesh
	   
	tPos = FragPos;
	tNormal = Normal;
	tMasks = vec4(1.0, 0, 0, 0);
	
	if(isPicking == 1){
		FragColor = vec4(_id / (255 * 2), 1);
	}
    if(isPicking == 0){
        _shadow();
		FragColor = vec4(getFragment(uv, 8), 0);
	}	
	
}
