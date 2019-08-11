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
uniform sampler2D diffuse0;
uniform sampler2D diffuse1;
uniform sampler2D diffuse2;

uniform sampler2D texture_mask;

uniform int isPicking;

vec3 getFragment(vec2 uv, int multiplyer){
    float mask = texture(texture_mask, uv).r;
    vec3 result;

    if(mask == 0.0)
        result = texture(diffuse0, uv * multiplyer).xyz;
    if(mask == 2.0 / 255)
        result = texture(diffuse1, uv * multiplyer).xyz;
    if(mask < 2.0 / 255 && mask > 0){
        result = mix(texture(diffuse0, uv * multiplyer).xyz, result = texture(diffuse1, uv * multiplyer).xyz, mask / 2 * 255);
    }
        
    return result;
}

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 1.0, 1.0); //Default value
	vec3 Normal = InNormal; //defaultly, use normals from mesh
	   
            
	//tDiffuse = vec4(result, result_shininess);
	tPos = FragPos;
	tNormal = Normal;
	tMasks = vec4(1.0, 0, 0, 0);

	if(isPicking == 1){
		FragColor = vec4(_id / (255 * 2), 1);
	}
    if(isPicking == 0){
		FragColor = vec4(getFragment(uv, 4), 0);
	}	
	
}
