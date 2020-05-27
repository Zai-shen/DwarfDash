#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 viewProjMatrix;

void main()
{
	gl_Position = viewProjMatrix * modelMatrix * vec4(aPos, 1.0);
}