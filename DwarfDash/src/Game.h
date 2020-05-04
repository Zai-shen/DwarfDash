#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"
#include "Level.h"

using namespace std;

class Game {

public:

	shared_ptr<Shader> primaryShader;
	shared_ptr<Material> woodTextureMaterial;
	shared_ptr<Material> brickTextureMaterial;

	Game();
	~Game();

	void init();

	void update();

	void draw();

	void reset();

	void createInitialGeometry();

	void addGeometry(Geometry* geometry);

private:
	//std::vector<Gameobject*> objects;

	Level* level1 = new Level();
	Level* level2 = new Level();

	Level* currentLevel = level1;

	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int activeGameState = GAME_STATE_IDLE;

	shared_ptr<Texture> woodTexture;
	shared_ptr<Texture> brickTexture;

	vector<Geometry*> geometryObjects;
};