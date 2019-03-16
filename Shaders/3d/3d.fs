#version 150 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (location = 0) out vec4 tDiffuse;
layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;

out vec4 FragColor;

in vec3 FragPos;
in vec3 InNormal;
in vec2 UVCoord;

//textures
uniform sampler2D diffuse;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasSpecularMap;

uniform float material_shininess;

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 0.078, 0.574); //Default value
	vec3 Normal = InNormal; //defaultly, use normals from mesh
	float result_shininess = material_shininess;
	
	if(hasDiffuseMap)
		result = texture(diffuse, uv).xyz ;
		
    if(hasNormalMap)
        Normal = texture(normal_map, uv).xyz;
        
    if(hasSpecularMap)
        result_shininess *= texture(specular_map, uv).x;
            
	tDiffuse = vec4(result, result_shininess);
	tPos = FragPos;
	tNormal = Normal;
}
