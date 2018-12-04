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
in vec3 n_Normal;
in mat3 TBN;
in vec4 uvObject;

//textures
uniform sampler2D diffuse;

uniform Light lights[100];
uniform int lights_amount = 0;

uniform bool hasDiffuseMap;

uniform bool processLights;

uniform vec3 amb_color;

void main(){

	vec2 uv = UVCoord;

	
	vec3 result = vec3(1.0, 0.078, 0.574); //Default value
	if(hasDiffuseMap)
		result = texture(diffuse, uv).xyz ;
		
       
	
	FragColor = vec4(result, 1);
}
