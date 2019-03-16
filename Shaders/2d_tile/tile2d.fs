#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tTransparent;

out vec4 FragColor;

in vec3 FragPos;
in vec2 UVCoord;

//textures
uniform sampler2D diffuse;
uniform sampler2D transparent;

uniform int total_rows; //rows in texture atlas
uniform int total_cols; //columns in texture atlas
uniform int selected_row; //current row
uniform int selected_col; //current column
//Animation toggler
uniform bool animated;

uniform bool hasDiffuseMap;
uniform bool hasTransparentMap;

void main(){

	vec2 uv = UVCoord;
	if(animated){ //if animation turned on
        uv.x = (uv.x + selected_row) / total_rows;
        uv.y = (uv.y + selected_col) / total_cols;
    }
	
	vec3 result = vec3(1.0, 0.078, 0.574); //Default value
	if(hasDiffuseMap) //if diffuse texture picked
		result = texture(diffuse, uv).xyz;
		
	//FragColor = vec4(result, 1);
	tDiffuse = vec4(result, 1);
	tPos = FragPos;
	
	if(hasTransparentMap){
        tTransparent = texture(transparent, uv);
	}else{
        tTransparent = vec4(1,1,1,1);
	}
}
