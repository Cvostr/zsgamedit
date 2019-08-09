#version 450

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

uniform vec3 cam_position;

void main(){
    vec4 Diffuse = texture(tDiffuse, UVCoord);
    vec3 FragPos = texture(tPos, UVCoord).rgb;
    vec3 Normal = texture(tNormal, UVCoord).rgb;
    
    vec3 result = Diffuse.xyz;

    float specularFactor = Diffuse.w; //Get factor in A channel
    vec3 camToFragDirection = normalize(cam_position - FragPos);
    
    for(int lg = 0; lg < lights_amount; lg ++){
		if(lights[lg].type == LIGHTSOURCE_DIR){
			float lightcoeff = max(dot(Normal, normalize(lights[lg].dir)), 0.0) * lights[lg].intensity;
			vec3 rlight = lightcoeff * lights[lg].color;
			
			//Specular calculation
            vec3 lightDirReflected = reflect(normalize(-lights[lg].dir), Normal);
            float angle = max(dot(camToFragDirection, lightDirReflected), 0.0);
            rlight += pow(angle, 32) * specularFactor * lights[lg].color;
			
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
