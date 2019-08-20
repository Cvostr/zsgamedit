#version 420 core

#define LIGHTSOURCE_NONE 0
#define LIGHTSOURCE_DIR 1
#define LIGHTSOURCE_POINT 2
#define LIGHTSOURCE_SPOT 3

#define MAX_LIGHTS_NUM 150

struct Light{ //size = 64
	int type; //size = 4, offset = 0
    float range; // size = 4, offset = 4
	float intensity; //size = 4, offset = 8
	float spot_angle; //size = 4, offset = 12
	vec3 pos; //size = 16, offset = 16 
	vec3 dir; //size = 16, offset = 32
	vec3 color; //size = 16, offset = 48
};
//Output Vec
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 UVCoord;

//textures
uniform sampler2D diffuse;
uniform sampler2D diffuse2;

    uniform int total_rows; //rows in texture atlas
    uniform int total_cols; //columns in texture atlas
    uniform int selected_row; //current row
    uniform int selected_col; //current column
    //Animation toggler
    uniform bool animated;

uniform bool hasDiffuseMap;
uniform bool hasDiffuseMap2;

layout (std140, binding = 1) uniform Lights{
    uniform Light lights[MAX_LIGHTS_NUM];
    uniform int lights_amount;
};

void main(){

	vec2 uv = UVCoord;
	if(animated){ //if animation turned on
        uv.x = (uv.x + selected_row) / total_rows;
        uv.y = (uv.y + selected_col) / total_cols;
    }
	vec4 _diffuse;
	vec3 result = vec3(1.0, 0.078, 0.574); //Default value
	if(hasDiffuseMap){ //if diffuse texture picked
		_diffuse = texture(diffuse, uv);
		result = _diffuse.xyz;
    }
	if(hasDiffuseMap2){ //if diffuse2 texture picked
		vec4 sec = texture(diffuse2, uv);	
        result = mix(result, sec.xyz, sec.a);
    }

    for(int lg = 0; lg < lights_amount; lg ++){
            
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
    
    FragColor = vec4(result, _diffuse.a);
}
