#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particleColor;

// Ouput data
out vec4 color;

uniform sampler2D textureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture( textureSampler, UV ) * particleColor;

}