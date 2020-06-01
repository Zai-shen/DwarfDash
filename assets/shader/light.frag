#version 430 core
/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

in VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

out vec4 color;

uniform vec3 camera_world;
uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
uniform float specularAlpha;

//uniform sampler2D diffuseTexture;

uniform sampler2D texture_diffuse1; 
uniform sampler2D texture_specular1; 
uniform sampler2D texture_normal1; 

uniform struct DirectionalLight {
	vec3 color;
	vec3 direction;
} dirL;

uniform struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
} pointL;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

//vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, float diffuseF, vec3 specularC, float specularF, float alpha, bool attenuate, vec3 attenuation) {
vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, float diffuseF, vec3 specularC, vec3 specularF, float alpha, bool attenuate, vec3 attenuation) {
	float d = length(l);
	l = normalize(l);
	float att = 1.0;	
	if(attenuate) att = 1.0f / (attenuation.x + d * attenuation.y + d * d * attenuation.z);
	vec3 r = reflect(-l, n);
	return (diffuseF * diffuseC * max(0, dot(n, l)) + specularF * specularC * pow(max(0, dot(r, v)), alpha)) * att; 
}

void main() {	
	vec3 n = normalize(vert.normal_world);
	vec3 v = normalize(camera_world - vert.position_world);
	
	//vec3 texColor = texture(diffuseTexture, vert.uv).rgb; // ecg
	vec3 texColor = texture(texture_diffuse1, vert.uv).rgb; // my own

	color = vec4(texColor * materialCoefficients.x, 1); // ambient
	
	// add directional light contribution
	//color.rgb += phong(n, -dirL.direction, v, dirL.color * texColor, materialCoefficients.y, dirL.color, materialCoefficients.z, specularAlpha, false, vec3(0));
	color.rgb += phong(n, -dirL.direction, v, dirL.color * texColor, materialCoefficients.y, dirL.color, vec3(texture(texture_specular1, vert.uv).rgb), specularAlpha, false, vec3(0));
			
	// add point light contribution
	//color.rgb += phong(n, pointL.position - vert.position_world, v, pointL.color * texColor, materialCoefficients.y, pointL.color, materialCoefficients.z, specularAlpha, true, pointL.attenuation);
	color.rgb += phong(n, pointL.position - vert.position_world, v, pointL.color * texColor, materialCoefficients.y, pointL.color,  vec3(texture(texture_specular1, vert.uv).rgb), specularAlpha, true, pointL.attenuation);
}

