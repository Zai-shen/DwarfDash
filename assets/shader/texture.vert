#version 430 core
/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

//layout(location = 3) in vec3 tangent;
//layout(location = 4) in vec3 bitangent;

out VertexData {
	vec3 position_world; // fragpos
	vec3 normal_world;
	vec2 uv;
} vert;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat3 normalMatrix;

void main() {
	// normal mapping without tangent/bitangent

	vert.normal_world = normalMatrix * normal;
	vert.uv = uv;
	
	vec4 position_world_ = modelMatrix * vec4(position, 1);
	vert.position_world = position_world_.xyz;
	
	gl_Position = viewProjMatrix * position_world_;
}