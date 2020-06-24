#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "FPSCamera.h"
#include "Utils.h"
#include "DDSLoader.h"
#include <stb_image.h>

using namespace std;

class FlareTexture
{
public:

	//position
	glm::vec3 position;
	
	FlareTexture();

	FlareTexture(std::shared_ptr<Shader> shader, string texture, FPSCamera* camera, glm::vec2 scaleTex = glm::vec2(1.f,1.f), glm::vec3 pos = glm::vec3(0.0f,1.0f,-1.0f));

	~FlareTexture();

	// update all particles
	void update(float dt);

	// render all particles
	void draw();

	FPSCamera* camera;

private:
	//size
	glm::vec2 scaleTexture;

	//texture
	GLuint texId;

	std::shared_ptr<Shader> shader;

	// render state
	GLuint VAO;
	GLuint billboard_vertex_buffer;

	// initializes buffer and vertex attributes
	void init(string texture);
};