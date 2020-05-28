#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 view;
uniform mat4 projection;

void main() {
	TexCoords = uv;
	gl_Position = projection * view * modelMatrix * vec4(position, 1.0);
}

// this works with ecg cam
//#version 430 core
//
//layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 normal;
//layout(location = 2) in vec2 uv;
//
//out vec2 TexCoords;
//
//layout(location = 0) uniform mat4 modelMatrix;
//uniform mat4 viewProjMatrix;
//uniform mat3 normalMatrix;
//
//void main() {
//	TexCoords = uv;
//	gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1.0);
//}