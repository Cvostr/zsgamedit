#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

#define LIGHTSOURCE_NONE 0
#define LIGHTSOURCE_DIR 1
#define LIGHTSOURCE_POINT 2
#define LIGHTSOURCE_SPOT 3

struct Light{
	int type;
	vec3 pos;
	vec3 dir;
	vec3 color;
	float range;
	float intensity;
	//float spot_angle;
	//float spot_out_angle;
};

out vec4 FragColor;

in vec2 UVCoord;

//textures
uniform sampler2D tDiffuse;
uniform sampler2D tNormal;
uniform sampler2D tPos;
uniform sampler2D tTransparent;

uniform int lights_amount;
uniform Light lights[100];


void main(){

	vec3 result = texture(tDiffuse, UVCoord).xyz;
    vec3 FragPos = texture(tPos, UVCoord).rgb;
    vec3 Normal = texture(tNormal, UVCoord).rgb;
    

    for(int lg = 0; lg < lights_amount; lg ++){
		if(lights[lg].type == LIGHTSOURCE_DIR){
			float lightcoeff = max(dot(Normal, normalize(lights[lg].dir)), 0.0) * lights[lg].intensity;
			vec3 rlight = lightcoeff * lights[lg].color;
			result += rlight;
		}
		if(lights[lg].type == LIGHTSOURCE_POINT){
			float dist = length(lights[lg].pos - FragPos);
			float factor = 1.0 / ( 1.0 + 1.0 / lights[lg].range * dist + 1.0 / lights[lg].range * dist * dist) * lights[lg].intensity;
			result += lights[lg].color * factor;
		}
	}
		
	FragColor = vec4(result, 1);
}
