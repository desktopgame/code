// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;
in mat4 fragWorldMat;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;
struct Material {
    sampler2D mDiffuse;
    sampler2D mSpecular;
    float mShininess;
}; 
// Create a struct for directional light
struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};
struct PointLight {    
    vec3 mPosition;
    
    float mConstant;
    float mLinear;
    float mQuadratic;  

    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

// Uniforms for lighting
// Camera position (in world space)
uniform vec3 uCameraPos;
// Specular power for this surface
uniform float uSpecPower;
// Ambient light level
uniform vec3 uAmbientLight;
uniform Material uMaterial;

// Directional Light
uniform DirectionalLight uDirLight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// Surface normal
	vec3 N = normalize(fragNormal);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        Phong += CalcPointLight(pointLights[i], fragNormal, fragWorldPos, normalize(uCameraPos - fragWorldPos));
	}
	// Final color is texture color times phong light (alpha = 1)
    outColor = vec4(Phong, 1.0f);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec4 ltmp = vec4(light.mPosition, 1.0);
	ltmp = ltmp * fragWorldMat;
	vec3 ltmp3 = ltmp.xyz;

    vec3 lightDir = normalize(ltmp3 - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.mShininess);
    // attenuation
    float distance    = length(ltmp3 - fragPos);
    float attenuation = 1.0 / (light.mConstant + light.mLinear * distance + 
  			     light.mQuadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.mAmbient  * vec3(texture(uMaterial.mDiffuse, fragTexCoord));
    vec3 diffuse  = light.mDiffuse  * diff * vec3(texture(uMaterial.mDiffuse, fragTexCoord));
    vec3 specular = light.mSpecular * spec * vec3(texture(uMaterial.mSpecular, fragTexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 