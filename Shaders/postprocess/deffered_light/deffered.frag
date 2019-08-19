#version 420 core

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
};

out vec4 FragColor;

in vec2 UVCoord;

//textures
uniform sampler2D tDiffuse;
uniform sampler2D tNormal;
uniform sampler2D tPos;
uniform sampler2D tTransparent;
uniform sampler2D tMasks;

uniform int lights_amount;
uniform Light lights[100];

uniform vec3 ambient_color;

uniform vec3 cam_position;

void main(){

    vec4 Diffuse = texture(tDiffuse, UVCoord);
    vec3 FragPos = texture(tPos, UVCoord).rgb;
	vec3 Normal = texture(tNormal, UVCoord).rgb;
	vec4 Transparent = texture(tTransparent, UVCoord);
	vec4 Masks = texture(tMasks, UVCoord);   	

    vec3 result = Diffuse.xyz;

    //Check, if fragment isn't skybox
    if(Masks.r == 1){
        result *= (1 - Masks.g);
        result *= ambient_color;
        
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
            if(lights[lg].type == LIGHTSOURCE_SPOT){
                
                vec3 vec_dir = normalize(lights[lg].dir);
                vec3 vec_frag_light = (lights[lg].pos - FragPos);
                
                float vangle = dot(vec_dir, normalize(vec_frag_light));
                
                float spot_angle = cos(lights[lg].spot_angle);
                float spot_out_angle = cos(spot_angle + 0.2);
                
                
                float epsilon   = spot_angle - spot_out_angle;
                float intensity = clamp((vangle - spot_out_angle) / epsilon, 0.0, 1.0);
                intensity = intensity;
                result += lights[lg].color * intensity * lights[lg].intensity * (lights[lg].range / length(vec_frag_light));
                
            }
        }
	}

	FragColor = vec4(result, 1);
}
