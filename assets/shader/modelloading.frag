#version 430 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D textureUnit;

void main(){
	fragColor = texture(textureUnit, TexCoords);
}