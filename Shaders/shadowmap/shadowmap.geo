layout(triangles, invocations = 3) in; 
layout(triangles_strip, max_vertices = 3) out;

layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform mat4 LightProjectionMat; // 16 * 4
    uniform mat4 LightViewMat; // 16 * 4
    uniform mat4 LightViewMat1; // 16 * 4
    uniform mat4 LightViewMat2; // 16 * 4
    uniform float shadow_bias; //4
    uniform bool hasShadowMap; //4
    uniform int shadowmap_Width; //4
    uniform int shadowmap_Height; //4
};

//Cascaded projection matrices are stored here
layout (std140, binding = 9) uniform InstMatrices{
    uniform mat4 inst_transform[1000];
};

void main() { 
    // for every vertex in the triangle... 
    for(int i = 0; i < gl_in.length(); i++) { 
        gl_ViewportIndex = gl_InvocationID; 
        // add here any other viewport-specific transform 
        gl_Position = LightProjectionMat * inst_transform[gl_InvocationID] * gl_in[i].gl_Position; 
        EmitVertex(); 
    }
    EndPrimitive();
} 

//vec4 pos = LightProjectionMat * inst_transform[gl_InstanceID] * object_transform * bone_t * vec4(position, 1.0);