#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"
#include "Level.h"
#include "Gameobject.h"
#include "Model.h"

using namespace std;

class Game {

public:

	shared_ptr<Shader> primaryShader;
	shared_ptr<Shader> modelShader;
	shared_ptr<Material> woodTextureMaterial;
	shared_ptr<Material> brickTextureMaterial;

	PxPhysics* gPhysics;
	PxScene* gScene;

	Game();
	~Game();

	void init();

	void update();

	void draw();

	void reset();

	void addGameobject(Gameobject* gameObject);

	Level* getCurrentLevel();

private:

	Level* level1 = new Level();
	Level* level2 = new Level();
	Level* level3 = new Level();
	Level* currentLevel = level1;

	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int activeGameState = GAME_STATE_IDLE;

	shared_ptr<Texture> woodTexture;
	shared_ptr<Texture> brickTexture;

	void initShaders();
	void initTextures();
	void initMaterials();
	void initLevels();
	void initLevel1();
	void initLevel2();
	void initLevel3();


};