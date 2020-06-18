#version 430 core
layout (location = 0) in vec3 vertex; //New: vec3 pos,no tex / <vec2 position, vec2 texCoords>

out vec4 ParticleColor;

//uniform mat4 projection;
uniform mat4 viewProjMatrix;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 0.5f;
    ParticleColor = color;
    //gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
	gl_Position = viewProjMatrix * vec4((vertex * scale) + offset, 1.0);
}