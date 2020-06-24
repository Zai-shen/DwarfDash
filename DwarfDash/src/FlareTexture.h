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
	GLuint Texture;

	FlareTexture();
	~FlareTexture();

	// update all particles
	void update(float dt);

	// render all particles
	void draw();

	FPSCamera* camera;

private:


	// render state
	//std::shared_ptr<Shader> shader;
	GLuint VAO;

	// initializes buffer and vertex attributes
	void init();
};