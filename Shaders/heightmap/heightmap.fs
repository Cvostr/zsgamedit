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
in vec3 _id;

//textures
uniform sampler2D diffuse[8];

uniform sampler2D texture_mask;
uniform sampler2D texture_mask1;

uniform int isPicking;

float getFactor(int id, vec2 uv){
    vec4 mask;
	int maskid = id / 4;
	if(maskid == 0)
        mask = texture(texture_mask, uv);
    if(maskid == 1)
        mask = texture(texture_mask1, uv);
    
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
        
    }
}

vec3 getFragment(vec2 uv, int multiplyer){
    vec3 result = vec3(0,0,0);

    for(int i = 0; i < 8; i ++){
        float factor = getFactor(i, uv);
        vec3 diffuse = getDiffuse(i, uv, multiplyer);
        
        result = mix(result, diffuse, factor);
    }
        
    return result;
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
		FragColor = vec4(getFragment(uv, 8), 0);
	}	
	
}
