/*
This code is based on: https://learnopengl.com/In-Practice/Text-Rendering
*/
#pragma once

#include "Shader.h"
#include "Utils.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRendering {

public: 
	TextRendering(Shader &textShader, float width, float height);
	
	~TextRendering();

	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
	std::map<GLchar, Character> Characters;
	unsigned int VAO;
	unsigned int VBO;

	glm::mat4 _projection;	
	Shader _textShader;

};
