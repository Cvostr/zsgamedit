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
	float spot_angle;
	float spot_out_angle;
};

out vec4 FragColor;

in vec3 FragPos;
in vec2 UVCoord;

//textures
uniform sampler2D tDiffuse;
uniform sampler2D tNormal;
uniform sampler2D tPos;

uniform Light lights[100];
uniform int lights_amount = 0;

void main(){

	vec3 result;
    result = texture(tDiffuse, UVCoord).xyz ;
		
	FragColor = vec4(result, 1);
}
