#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "FPSCamera.h"
#include "Utils.h"
#include "FlareTexture.h"
#include "Player.h"

using namespace std;

class FlareRenderer
{
public:

	//position
	glm::vec2 screenPosition;
	//size
	float scale;

	FlareRenderer(std::shared_ptr<Shader> shader, FPSCamera* camera, Player* player);

	~FlareRenderer();

	// update all particles
	void update(float dt);

	// render all particles
	void draw();

	FPSCamera* camera;
	Player* player;

private:

	std::vector<FlareTexture*> flares;
	// render state
	std::shared_ptr<Shader> shader;

	GLuint VAO;

	// initializes buffer and vertex attributes
	void init();
};