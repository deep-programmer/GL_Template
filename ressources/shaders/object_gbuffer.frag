#version 330

// Input: tangent space matrix, position (view space) and uv coming from the vertex shader
in INTERFACE {
    mat3 tbn;
	vec2 uv;
} In ;

uniform sampler2D textureColor;
uniform sampler2D textureNormal;
uniform sampler2D textureEffects;

uniform int materialId;

// Output: the fragment color
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragEffects;


void main(){
	
	vec2 localUV = In.uv;
	
	// Compute the normal at the fragment using the tangent space matrix and the normal read in the normal map.
	vec3 n = texture(textureNormal,localUV).rgb;
	n = normalize(n * 2.0 - 1.0);
	
	// Store values.
	fragColor.rgb = texture(textureColor, localUV).rgb;
	fragColor.a = float(materialId)/255.0;
	fragNormal.rgb = normalize(In.tbn * n)*0.5+0.5;
	fragEffects.rgb = texture(textureEffects,localUV).rgb;
	
}