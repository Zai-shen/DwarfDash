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

	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vert;

out vec4 color;

uniform vec3 camera_world;
uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
uniform float specularAlpha;

//uniform sampler2D diffuseTexture;

uniform sampler2D texture_diffuse1; 
uniform sampler2D texture_specular1; 
uniform sampler2D texture_normal1; 

uniform bool normalMapping;

uniform struct DirectionalLight {
	vec3 color;
	vec3 direction;
} dirL;

uniform struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
} pointL;

uniform PointLight pointLights[8];

vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, float diffuseF, vec3 specularC, vec3 specularF, float alpha, bool attenuate, vec3 attenuation) {
	float d = length(l);
	l = normalize(l);
	float att = 1.0;	
	if(attenuate) att = 1.0f / (attenuation.x + d * attenuation.y + d * d * attenuation.z);
	vec3 r = reflect(-l, n);
	return (diffuseF * diffuseC * max(0, dot(n, l)) + specularF * specularC * pow(max(0, dot(r, v)), alpha)) * att; 
}

void main() {	

	vec3 n = normalize(vert.normal_world); // ecg framework (used if normapping is disabled)
		
	if(normalMapping) {
	    n = texture(texture_normal1, vert.uv).rgb;	   // obtain normal from normal map in range [0, 1]
		n = normalize(n * 2.0 - 1.0);  				   // transform normal vector to range [-1,1]
		//n = normalize(n);  				   // transform normal vector to range [-1,1]
	} 

	vec3 v = normalize(camera_world - vert.position_world); // ecg framework

	//vec3 v = normalize(vert.TangentLightPos - vert.TangentFragPos); // used for tangent/bitangent normal mapping

	vec3 texColor = texture(texture_diffuse1, vert.uv).rgb;
	color = vec4(texColor * materialCoefficients.x, 1); // ambient
	
	// add directional light contribution
	color.rgb += phong(n, -dirL.direction, v, dirL.color * texColor, materialCoefficients.y, dirL.color, vec3(texture(texture_specular1, vert.uv).rgb), specularAlpha, false, vec3(0));
			
	// add (multiple) point light contribution
	for(int i = 0; i < pointLights.length(); i++){
		color.rgb += phong(n, pointLights[i].position - vert.position_world, v,  pointLights[i].color * texColor, materialCoefficients.y,  pointLights[i].color,  vec3(texture(texture_specular1, vert.uv).rgb), specularAlpha, true,  pointLights[i].attenuation);
	}

}

