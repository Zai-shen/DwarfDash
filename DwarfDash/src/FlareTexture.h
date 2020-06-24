#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "FPSCamera.h"
#include "Utils.h"

using namespace std;

class FlareTexture
{
public:

	//position
	glm::vec2 screenPosition;
	//size
	float scale;
	//texture
	//GLuint texture;
	Texture texture;
	
	FlareTexture();

	FlareTexture(std::shared_ptr<Shader> shader, string texture, FPSCamera* camera, float scale = 1.f, glm::vec2 screenPos = glm::vec2(0.0f,0.0f));

	~FlareTexture();

	// update all particles
	void update(float dt);

	// render all particles
	void draw();

	FPSCamera* camera;

private:

	std::shared_ptr<Shader> shader;

	// render state
	GLuint VAO;
	GLuint billboard_vertex_buffer;

	// initializes buffer and vertex attributes
	void init();
};