#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"

using namespace std;

class Game {

public:

	shared_ptr<Shader> primaryShader;

	Game();
	~Game();

	void init();

	void update();

	void draw();

	void reset();

	void createGeometry();

private:
	//std::vector<Gameobject*> objects;

	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int activeGameState = GAME_STATE_IDLE;

	shared_ptr<Texture> woodTexture;
	shared_ptr<Texture> brickTexture;
	shared_ptr<Material> woodTextureMaterial;
	shared_ptr<Material> brickTextureMaterial;

	vector<Geometry*> geometryObjects;
};