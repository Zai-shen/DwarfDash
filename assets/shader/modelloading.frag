#version 430 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D textureUnit;

void main(){
	fragColor = texture(textureUnit, TexCoords);
}


// CODE VON  LEARNOPENGL.COM
//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoords;
//
//uniform sampler2D texture_diffuse1;
//
//void main()
//{    
//    FragColor = texture(texture_diffuse1, TexCoords);
//}